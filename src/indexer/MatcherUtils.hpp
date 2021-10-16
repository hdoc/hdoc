// Copyright 2019-2021 hdoc
// SPDX-License-Identifier: AGPL-3.0-only

#pragma once

#include "types/Symbols.hpp"
#include "clang/AST/Comment.h"
#include <filesystem>
#include <string>

/// @brief Update the name, line, and file of the decl
void fillOutSymbol(hdoc::types::Symbol& s, const clang::NamedDecl* d, const std::filesystem::path& rootDir);

/// @brief Find the parent namespace (either record or an actual namespace) of a decl
void findParentNamespace(hdoc::types::Symbol& s, const clang::NamedDecl* d);

/// @brief Check if a decl is defined in a non-existent file or in the set of ignored paths
bool isInBlacklist(const clang::Decl*              d,
                   const std::vector<std::string>& ignorePaths,
                   const std::filesystem::path&    rootDir);

/// @brief Check if the decl is in an anonymous namespace
bool isInAnonymousNamespace(const clang::Decl* d);

/// @brief Update FunctionSymbol.proto with the full prototype
std::string getFunctionSignature(hdoc::types::FunctionSymbol& f, const clang::FunctionDecl* function);

/// @brief Convert a clang::Expr to a string, like clang::Decl->getNameAsString()
std::string exprToString(const clang::Expr* expr, clang::PrintingPolicy printingPolicy);

/// @brief Build a SymbolID from a decl
hdoc::types::SymbolID buildID(const clang::NamedDecl* d);

/// @brief Get the Doxygen command name (i.e. brief, param, returns) from a CommandID
std::string getCommandName(const unsigned& CommandID);
std::string getParaCommentContents(const clang::comments::Comment* comment, clang::ASTContext& ctx);
std::string getCommentContents(const clang::comments::Comment* comment);
void        processRecordComment(hdoc::types::RecordSymbol&      cs,
                                 const clang::comments::Comment* comment,
                                 clang::ASTContext&              ctx);
void processEnumComment(hdoc::types::EnumSymbol& e, const clang::comments::Comment* comment, clang::ASTContext& ctx);
void processFunctionComment(hdoc::types::FunctionSymbol&    f,
                            const clang::comments::Comment* comment,
                            clang::ASTContext&              ctx);
