// Copyright 2019-2022 hdoc
// SPDX-License-Identifier: AGPL-3.0-only

#include "MatcherUtils.hpp"
#include "support/StringUtils.hpp"

#include "clang/AST/ASTContext.h"
#include "clang/AST/DeclTemplate.h"
#include "clang/Basic/FileManager.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Index/USRGeneration.h"
#include "clang/Lex/Lexer.h"

#include "spdlog/spdlog.h"

#include <filesystem>

/// When hdoc is run by multiple threads, we use a VFS (virtual file system) to access
/// files safely. The working directory of the parser is changed during indexing, and
/// other threads have no visibility of this. Consequently, canonical paths
/// generated in a non-VFS-aware way can be wrong.
/// This function is similar to one defined in clang, and gets the canonical path in a
/// VFS-aware way.
static llvm::Optional<std::string> getCanonicalPath(const clang::Decl* d) {
  const auto& sourceManager = d->getASTContext().getSourceManager();
  const auto* fileEntry     = sourceManager.getFileEntryForID(sourceManager.getFileID(d->getLocation()));

  if (!fileEntry) {
    return llvm::None;
  }

  llvm::SmallString<128> path = fileEntry->getName();
  if (!llvm::sys::path::is_absolute(path)) {
    if (auto ec = sourceManager.getFileManager().getVirtualFileSystem().makeAbsolute(path)) {
      spdlog::warn("Could not turn relative path '{}' to absolute: {}", path.c_str(), ec.message().c_str());
      return llvm::None;
    }
  }

  if (auto dir = sourceManager.getFileManager().getDirectory(llvm::sys::path::parent_path(path))) {
    const llvm::StringRef  dirName = sourceManager.getFileManager().getCanonicalName(*dir);
    llvm::SmallString<128> realPath;
    llvm::sys::path::append(realPath, dirName, llvm::sys::path::filename(path));
    return realPath.str().str();
  }

  return path.str().str();
}

template <typename T> static bool isParamAndHasName(T* param) {
  return param != nullptr && param->hasParamName();
}

/// This is used across all types of symbols (Function, Record, Namespace, etc.) to get the
/// vital information of the symbol
void fillOutSymbol(hdoc::types::Symbol& s, const clang::NamedDecl* d, const std::filesystem::path& rootDir) {
  s.name = d->getNameAsString();
  s.line = d->getASTContext().getSourceManager().getSpellingLineNumber(d->getLocation());

  const auto absPath = getCanonicalPath(d);
  if (!absPath) {
    spdlog::warn("Unable to get absolute path for {}", s.name);
    return;
  }
  s.file = std::filesystem::relative(*absPath, rootDir).string();
}

void findParentNamespace(hdoc::types::Symbol& s, const clang::NamedDecl* d) {
  const auto* dc = llvm::dyn_cast<clang::DeclContext>(d)->getParent();
  if (const auto* n = llvm::dyn_cast<clang::NamespaceDecl>(dc)) {
    s.parentNamespaceID = buildID(n);
  } else if (const auto* n = llvm::dyn_cast<clang::RecordDecl>(dc)) {
    s.parentNamespaceID = buildID(n);
  }
}

bool isInIgnoreList(const clang::Decl*              d,
                    const std::vector<std::string>& ignorePaths,
                    const std::filesystem::path&    rootDir) {
  const auto rawPath = std::filesystem::path(d->getASTContext().getSourceManager().getFilename(d->getLocation()).str());

  // If the decl has an empty path, it's probably compiler-generated so we ignore it
  if (rawPath == "") {
    return true;
  }

  const auto absPath = getCanonicalPath(d);
  if (!absPath) {
    spdlog::warn("Unable to get absolute path for a decl, ignoring it");
    return true;
  }

  // Ignore paths outside of the rootDir
  // ".." is used as a janky way to determine if the path is outside of rootDir since the canonicalized path
  // should not have any ".."s in it
  const std::string relPath = std::filesystem::relative(std::filesystem::path(*absPath), rootDir).string();
  if (relPath.find("..") != std::string::npos) {
    return true;
  }

  for (const auto& substr : ignorePaths) {
    if (absPath->find(substr) != std::string::npos) {
      return true;
    }
  }
  return false;
}

/// Decls in anonymous namespaces should not be documented
/// This function checks if a declaration is made in an anonymous namespace
/// or if any of its parents are
bool isInAnonymousNamespace(const clang::Decl* d) {
  const auto* dc = llvm::dyn_cast_or_null<clang::DeclContext>(d);
  if (dc == nullptr) {
    return false;
  }

  while (true) {
    dc = dc->getParent();
    if (dc == nullptr) {
      break;
    }
    if (const auto* ns = llvm::dyn_cast_or_null<clang::NamespaceDecl>(dc)) {
      if (ns->isAnonymousNamespace()) {
        return true;
      }
    }
  }

  return false;
}

std::string getRecordProto(const hdoc::types::RecordSymbol& c) {
  std::string proto;
  if (c.templateParams.size() > 0) {
    std::size_t count = 0;
    proto += "template <";
    for (const auto& tparam : c.templateParams) {
      if (count > 0) {
        proto += ", ";
      }
      if (tparam.templateType == hdoc::types::TemplateParam::TemplateType::TemplateTypeParameter) {
        proto += tparam.isTypename ? "typename" : "class";
        proto += tparam.isParameterPack ? "... " : " ";
        proto += tparam.name;
        // Get default argument if it exists
        proto += tparam.defaultValue.size() > 0 ? " = " + tparam.defaultValue : "";
      } else if (tparam.templateType == hdoc::types::TemplateParam::TemplateType::NonTypeTemplate) {
        proto += tparam.type;
        proto += tparam.isParameterPack ? "..." : "";
        proto += " " + tparam.name;
        // Get default argument if it exists
        proto += tparam.defaultValue.size() > 0 ? " = " + tparam.defaultValue : "";
      } else if (tparam.templateType == hdoc::types::TemplateParam::TemplateType::TemplateTemplateType) {
        proto += tparam.type;
        proto += tparam.isParameterPack ? "..." : "";
        proto += " " + tparam.name;
      }
      count++;
    }
    proto += "> ";
  }
  proto += c.type + " " + c.name;
  return proto;
}

std::string getFunctionSignature(hdoc::types::FunctionSymbol& f) {
  std::string signature;
  if (f.templateParams.size() > 0) {
    uint64_t count = 0;
    signature += "template <";
    for (const auto& tparam : f.templateParams) {
      signature += count > 0 ? ", " : "";
      if (tparam.templateType == hdoc::types::TemplateParam::TemplateType::TemplateTypeParameter) {
        signature += tparam.isTypename ? "typename" : "class";
        signature += tparam.isParameterPack ? "..." : "";
        signature += " " + tparam.name;
        // Get default argument if it exists
        signature += tparam.defaultValue.size() > 0 ? " = " + tparam.defaultValue : "";
      } else if (tparam.templateType == hdoc::types::TemplateParam::TemplateType::NonTypeTemplate) {
        signature += tparam.type;
        signature += tparam.isParameterPack ? "..." : "";
        signature += " " + tparam.name;
        // Get default argument if it exists
        signature += tparam.defaultValue.size() > 0 ? " = " + tparam.defaultValue : "";
      }
      count++;
    }
    signature += ">";
  }

  f.postTemplate = signature.size();

  // Various qualifiers
  signature += f.storageClass == clang::SC_Static ? "static " : "";
  signature += f.storageClass == clang::SC_Extern ? "extern " : "";
  signature += f.isInline ? "inline " : "";
  signature += f.isVirtual ? "virtual " : "";
  signature += f.isConstexpr ? "constexpr " : "";
  signature += f.isConsteval ? "consteval " : "";

  // Return type
  if (f.isCtorOrDtor == false) {
    signature += f.hasTrailingReturn ? "auto " : f.returnType.name + " ";
  }

  // Get the location of the first character of the function name
  f.nameStart = signature.size();

  // Name and opening bracket
  signature += f.name + "(";

  // Function parameters
  uint64_t count = 0;
  for (const auto& param : f.params) {
    signature += count > 0 ? ", " : "";
    signature += param.type.name;
    // Functions can have unnamed parameters, so don't add the space and name if it doesn't exist
    signature += param.name != "" ? " " + param.name : "";
    // Add default argument if it exists
    signature += param.defaultValue != "" ? " = " + param.defaultValue : "";
    count++;
  }

  // Add parentheses for variadic functions, along with closing bracket
  signature += f.params.size() > 0 && f.isVariadic ? ", ..." : "";
  signature += f.params.size() == 0 && f.isVariadic ? "..." : "";
  signature += ")";

  // Add CVR qualifiers
  signature += f.isConst ? " const" : "";
  signature += f.isVolatile ? " volatile" : "";
  signature += f.isRestrict ? " restrict" : "";

  // Add reference qualifier
  signature += f.refQualifier == clang::RQ_LValue ? " &" : "";
  signature += f.refQualifier == clang::RQ_RValue ? " &&" : "";

  // Add noexcept qualifier
  signature += f.isNoExcept ? " noexcept" : "";

  // Trailing return type goes last
  signature += f.hasTrailingReturn ? " -> " + f.returnType.name : "";

  return signature;
}

/// Basically getNameAsString() but for Clang::Expr instead of Clang::Decl
std::string exprToString(const clang::Expr* expr, clang::PrintingPolicy printingPolicy) {
  if (expr == nullptr) {
    return "";
  }
  printingPolicy.FullyQualifiedName  = 1;
  printingPolicy.SuppressScope       = 0;
  printingPolicy.PrintCanonicalTypes = 1;

  std::string              result;
  llvm::raw_string_ostream stream(result);
  expr->printPretty(stream, nullptr, printingPolicy);
  stream.flush();
  return result;
}

hdoc::types::SymbolID buildID(const clang::NamedDecl* d) {
  llvm::SmallString<128> USR;
  if (clang::index::generateUSRForDecl(d, USR)) {
    spdlog::error("Unable to generate USR for the given symbol with name {}", d->getNameAsString());
    return hdoc::types::SymbolID();
  } else {
    return hdoc::types::SymbolID(USR);
  }
}

std::string getCommandName(const unsigned& CommandID) {
  const clang::comments::CommandInfo* cmd = clang::comments::CommandTraits::getBuiltinCommandInfo(CommandID);
  return cmd ? cmd->Name : "";
}

std::string getParaCommentContents(const clang::comments::Comment* comment, clang::ASTContext& ctx) {
  std::string text;
  bool        prevCommentWasDoxygenCommand = false;
  for (auto c = comment->child_begin(); c != comment->child_end(); ++c) {
    if (const auto* icc = llvm::dyn_cast<clang::comments::InlineCommandComment>(*c)) {
      text += clang::Lexer::getSourceText(clang::CharSourceRange::getTokenRange(icc->getCommandNameRange()),
                                          ctx.getSourceManager(),
                                          ctx.getLangOpts())
                  .drop_front() // there seems to be a leading space before the command name by default
                  .str();
      prevCommentWasDoxygenCommand = true;
    } else if (const auto* tc = llvm::dyn_cast<clang::comments::TextComment>(*c)) {
      if (!tc->isWhitespace()) {
        std::string current_text = tc->getText().str();
        hdoc::utils::ltrim(current_text); // Remove leading whitespace (indentation)
        if (c == comment->child_begin() || prevCommentWasDoxygenCommand == true) {
          text += current_text;
          prevCommentWasDoxygenCommand = false;
        } else {
          text += " " + current_text; // Add a space if it isn't the first line of the comment
        }
      }
    }
  }
  return text;
}

std::string getCommentContents(const clang::comments::Comment* comment, clang::ASTContext& ctx) {
  std::string wholeText;
  for (auto c = comment->child_begin(); c != comment->child_end(); ++c) {
    wholeText += getParaCommentContents(*c, ctx);
  }
  return wholeText;
}

void processRecordComment(hdoc::types::RecordSymbol&      cs,
                          const clang::comments::Comment* comment,
                          clang::ASTContext&              ctx) {
  for (auto c = comment->child_begin(); c != comment->child_end(); ++c) {
    // Top-level paragraph comment is typically the object description
    if (const auto* paraComment = llvm::dyn_cast<clang::comments::ParagraphComment>(*c)) {
      if (paraComment->isWhitespace()) {
        continue;
      }
      cs.docComment += getParaCommentContents(paraComment, ctx) + " ";
    }

    // Only look for \brief doxygen command
    if (auto commandComment = llvm::dyn_cast<clang::comments::BlockCommandComment>(*c)) {
      const std::string commandName = getCommandName(commandComment->getCommandID());
      if (commandName == "brief") {
        cs.briefComment = getCommentContents(commandComment, ctx);
      }
    }

    if (const auto* tParamComment = llvm::dyn_cast<clang::comments::TParamCommandComment>(*c);
        isParamAndHasName(tParamComment)) {
      const std::string tParamName = tParamComment->getParamNameAsWritten().str();
      for (auto& tparam : cs.templateParams) {
        if (tparam.name == tParamName) {
          tparam.docComment = getCommentContents(tParamComment, ctx);
        }
      }
    }
  }
  hdoc::utils::rtrim(cs.briefComment);
  hdoc::utils::rtrim(cs.docComment);
}

void processEnumComment(hdoc::types::EnumSymbol& e, const clang::comments::Comment* comment, clang::ASTContext& ctx) {
  for (auto c = comment->child_begin(); c != comment->child_end(); ++c) {
    // Top-level paragraph comment is typically the object description
    if (const auto* paraComment = llvm::dyn_cast<clang::comments::ParagraphComment>(*c)) {
      if (paraComment->isWhitespace()) {
        continue;
      }
      e.docComment += getParaCommentContents(paraComment, ctx) + " ";
    }

    // Get verbatim comments
    if (const auto* verbatimComment = llvm::dyn_cast<clang::comments::VerbatimLineComment>(*c)) {
      e.docComment += verbatimComment->getText();
    }

    // Only look for \brief Doxygen commands
    if (const auto* commandComment = llvm::dyn_cast<clang::comments::BlockCommandComment>(*c)) {
      std::string commandName = getCommandName(commandComment->getCommandID());
      if (commandName == "brief") {
        e.briefComment = getCommentContents(commandComment, ctx);
      }
    }
  }
  hdoc::utils::rtrim(e.briefComment);
  hdoc::utils::rtrim(e.docComment);
}

void processFunctionComment(hdoc::types::FunctionSymbol&    f,
                            const clang::comments::Comment* comment,
                            clang::ASTContext&              ctx) {
  for (auto c = comment->child_begin(); c != comment->child_end(); ++c) {
    // Top-level paragraph comment is typically function description
    if (const auto* paraComment = llvm::dyn_cast<clang::comments::ParagraphComment>(*c)) {
      if (paraComment->isWhitespace()) {
        continue;
      }
      f.docComment += getParaCommentContents(paraComment, ctx) + " ";
    }

    // Match function parameter names with params in FunctionSymbol
    if (const auto* paramComment = llvm::dyn_cast<clang::comments::ParamCommandComment>(*c);
        isParamAndHasName(paramComment)) {
      const std::string paramName = paramComment->getParamNameAsWritten().str();
      for (auto& param : f.params) {
        if (param.name == paramName) {
          param.docComment = getCommentContents(paramComment, ctx);
        }
      }
    }

    if (const auto* tParamComment = llvm::dyn_cast<clang::comments::TParamCommandComment>(*c);
        isParamAndHasName(tParamComment)) {
      const std::string tParamName = tParamComment->getParamNameAsWritten().str();
      for (auto& tparam : f.templateParams) {
        if (tparam.name == tParamName) {
          tparam.docComment = getCommentContents(tParamComment, ctx);
        }
      }
    }

    // Look for \return, \returns, or \brief Doxygen commands
    if (const auto* commandComment = llvm::dyn_cast<clang::comments::BlockCommandComment>(*c)) {
      const std::string commandName = getCommandName(commandComment->getCommandID());
      if (commandName == "return" || commandName == "returns") {
        f.returnTypeDocComment = getCommentContents(commandComment, ctx);
      } else if (commandName == "brief") {
        f.briefComment = getCommentContents(commandComment, ctx);
      }
    }
  }
  hdoc::utils::rtrim(f.briefComment);
  hdoc::utils::rtrim(f.docComment);
}
