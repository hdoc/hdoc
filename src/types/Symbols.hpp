// Copyright 2019-2022 hdoc
// SPDX-License-Identifier: AGPL-3.0-only

#pragma once

#include "clang/AST/Type.h"
#include "clang/Basic/Specifiers.h"
#include "llvm/ADT/DenseMapInfo.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/Support/SHA1.h"

#include <string>
#include <vector>

namespace hdoc::types {
/// @brief A unique identifier for each symbol in hdoc's index, built from its clang USR
struct SymbolID {
  SymbolID() = default;

  /// @brief Constructs a SymbolID from a USR value by SHA1 hashing it
  SymbolID(llvm::StringRef USR) {
    const auto& hash = llvm::SHA1::hash(llvm::arrayRefFromStringRef(USR));
    this->hashValue |= static_cast<uint64_t>(hash[0]) << 56;
    this->hashValue |= static_cast<uint64_t>(hash[1]) << 48;
    this->hashValue |= static_cast<uint64_t>(hash[2]) << 40;
    this->hashValue |= static_cast<uint64_t>(hash[3]) << 32;
    this->hashValue |= static_cast<uint64_t>(hash[4]) << 24;
    this->hashValue |= static_cast<uint64_t>(hash[5]) << 16;
    this->hashValue |= static_cast<uint64_t>(hash[6]) << 8;
    this->hashValue |= static_cast<uint64_t>(hash[7]);
  }

  /// @brief Returns the raw hash value for this SymbolID
  uint64_t raw() const {
    return this->hashValue;
  }
  bool operator==(const SymbolID& rhs) const {
    return this->hashValue == rhs.hashValue;
  }

  /// @brief Returns the SymbolID as a hex string, prepending leading zeros if needed
  std::string str() const {
    auto          str  = llvm::utohexstr(this->hashValue);
    const uint8_t diff = 16 - str.size();
    for (uint32_t i = 0; i < diff; i++) {
      str.insert(str.begin(), '0');
    }
    return str;
  }

  uint64_t hashValue = 0; ///< USR value hashed into an integer
};

/// @brief Base class for all other types of symbols
struct Symbol {
  std::string           name;              ///< Function name, record name, enum name etc.
  std::string           briefComment;      ///< Text following @brief or \brief command
  std::string           docComment;        ///< All other Doxygen text attached to this symbol's documentation
  hdoc::types::SymbolID ID;                ///< Unique identifier for this Symbol
  std::string           file;              ///< File where this Symbol is declared, relative to source root
  std::uint64_t         line;              ///< Line number in the file
  hdoc::types::SymbolID parentNamespaceID; ///< ID of the parent namespace (or record)

  /// @brief Comparison operator sorts alphabetically by symbol name
  bool operator<(const Symbol& s) const {
    return this->name < s.name;
  }
};

/// @brief Represents a possible reference to another Symbol that may or may not be in the Index.
/// Used to represent cross-links to function parameters, return types, or record member variables.
struct TypeRef {
  hdoc::types::SymbolID id;   ///< Possible SymbolID of this type.
  std::string           name; ///< Name of the type
};

/// @brief Represents a function parameter
struct TemplateParam {
  enum class TemplateType {
    TemplateTypeParameter,
    TemplateTemplateType,
    NonTypeTemplate,
  };
  TemplateType templateType;

  std::string name;                    ///< Name given to the parameter
  std::string type;                    ///< Type given to the parameter (if any)
  std::string docComment;              ///< Any comment attached to this param using @tparam or \tparam
  std::string defaultValue;            ///< The default value for this param, if it exists
  bool        isParameterPack = false; ///< Is this template a parameter pack, i.e. "typename..."
  bool        isTypename      = false; ///< Was this template declared with "typename" or "class"?
};

/// @brief Represents a member variable of a record
struct MemberVariable {
  bool                   isStatic = false;           ///< Is this member variable marked static?
  std::string            name;                       ///< Name of the member variable
  hdoc::types::TypeRef   type;                       ///< Type of the string, i.e. int or a struct name
  std::string            defaultValue;               ///< Default value, usually an int
  std::string            docComment;                 ///< Any comment attached to this decl
  clang::AccessSpecifier access = clang::AS_private; ///< Access type, i.e. public/protected/private
};

/// @brief Describes a record, such as a struct, class, or union
struct RecordSymbol : public Symbol {
  /// @brief Represents a record that is being inherited from
  struct BaseRecord {
    hdoc::types::SymbolID  id;     ///< ID of the record that's being inherited from
    clang::AccessSpecifier access; ///< Type of inheritance, i.e. public/protected/private
    std::string            name;   ///< Name of the record, used only for base records in std:: which aren't indexed
  };

  std::string                        type;           ///< i.e. struct/class/union
  std::string                        proto;          ///< Full class prototype, including
  std::vector<MemberVariable>        vars;           ///< All of this record's member variables
  std::vector<hdoc::types::SymbolID> methodIDs;      ///< All of this record's methods
  std::vector<BaseRecord>            baseRecords;    ///< All of the records this record inherits from
  std::vector<TemplateParam>         templateParams; ///< All of the template parameters for this record

  std::string url() const {
    return "r" + this->ID.str() + ".html";
  }
};

/// @brief Represents a function parameter
struct FunctionParam {
  std::string          name;         ///< Name given to the parameter
  hdoc::types::TypeRef type;         ///< Type of the parameter, i.e. "int"
  std::string          docComment;   ///< Any comment attached to this param using @param or \param
  std::string          defaultValue; ///< The default value for this param, if it exists
};

/// @brief Symbol representing a function or member function
struct FunctionSymbol : public Symbol {
public:
  bool                       isRecordMember    = false; ///< Is it a method?
  bool                       isConstexpr       = false; ///< Is it marked constexpr?
  bool                       isConsteval       = false; ///< Is it marked consteval?
  bool                       isInline          = false; ///< Is it marked inline?
  bool                       isConst           = false; ///< Is it marked const?
  bool                       isVolatile        = false; ///< Is it marked volatile?
  bool                       isRestrict        = false; ///< Is it marked restrict?
  bool                       isVirtual         = false; ///< Is it a virtual function?
  bool                       isVariadic        = false; ///< Does it have a "..." parameter?
  bool                       isNoExcept        = false; ///< Is it marked noexcept?
  bool                       hasTrailingReturn = false; ///< Does use the funky `auto func() -> int {}` syntax?
  bool                       isCtorOrDtor      = false; ///< Is it a record constructor or destructor
  uint64_t                   nameStart         = 0;     ///< Position of the first character of the name
  uint64_t                   postTemplate      = 0; ///< Position of the first character after all the template magic
  clang::AccessSpecifier     access            = clang::AS_public; ///< Is the function public/protected/private
  clang::StorageClass        storageClass      = clang::SC_None;   ///< Is this function marked static or extern;
  clang::RefQualifierKind    refQualifier = clang::RQ_None; ///< Refqualifier of this function, if any, ex. void get() &
  std::string                proto;      ///< Function prototype, including template, return type, name, and params
  hdoc::types::TypeRef       returnType; ///< Return type of the function, ex. "int"
  std::string                returnTypeDocComment; ///< Any comment attached to a @return(s) or \return(s) command
  std::vector<FunctionParam> params;               ///< All of the template parameters for this function
  std::vector<TemplateParam> templateParams;       ///< All of the parameters for this function

  std::string url() const {
    return "f" + this->ID.str() + ".html";
  }
};

/// @brief Represents the values inside an enum
struct EnumMember {
  int64_t     value;      ///< Integer value this member resolves to
  std::string name;       ///< Name of the value
  std::string docComment; ///< Any comment attached to this value
};

/// @brief Represents an enum or scoped enum (enum class/struct)
struct EnumSymbol : public Symbol {
public:
  std::string             type = ""; ///< "class" for enum class, "struct" for enum struct, otherwise ""
  std::vector<EnumMember> members;   ///< All of this enum's values

  std::string url() const {
    return "e" + this->ID.str() + ".html";
  }
};

/// @brief Represents a namespace
struct NamespaceSymbol : public Symbol {
public:
  std::vector<hdoc::types::SymbolID> records    = {}; ///< All of the records in this namespace
  std::vector<hdoc::types::SymbolID> namespaces = {}; ///< All of the other namespaces in this namespace
  std::vector<hdoc::types::SymbolID> enums      = {}; ///< All of the enums in this namespace

  std::string url() const {
    return "n" + this->ID.str() + ".html";
  }
};

} // namespace hdoc::types

namespace std {
/// @brief Function to allow for SymbolID to be hashed in an std::unordered_map
template <> struct hash<hdoc::types::SymbolID> {
  std::size_t operator()(const hdoc::types::SymbolID& ID) const {
    return ID.raw();
  }
};
} // namespace std
