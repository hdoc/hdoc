// Copyright 2019-2023 hdoc
// SPDX-License-Identifier: AGPL-3.0-only

#include "Matchers.hpp"
#include "MatcherUtils.hpp"
#include "types/Symbols.hpp"
#include "clang/AST/Comment.h"
#include "clang/Lex/Lexer.h"

#include <string>

/// @brief Try to get a SymbolID from a QualType, and return an empty SymbolID if it's not possible
static hdoc::types::SymbolID getTypeSymbolID(const clang::QualType& typ) {
  // Get a TagDecl from the QualType, stripping pointers and references if needed.
  // Pointers and references look like different types to clang. If we want to
  // have working links in our documentation, types need to have consistent IDs
  // regardless if they are pointers, references, or templates (shown below).
  const clang::TagDecl* ret = NULL;
  if (const clang::TagDecl* decl = typ->getAsTagDecl()) {
    ret = decl;
  } else if (typ->isPointerType() && typ->getPointeeType()->getAsTagDecl()) {
    ret = typ->getPointeeType()->getAsTagDecl();
  } else if (typ->isReferenceType() && typ.getNonReferenceType()->getAsTagDecl()) {
    ret = typ.getNonReferenceType()->getAsTagDecl();
  } else {
    ret = NULL;
  }

  // Remove the template, if applicble.
  // If the type is a specialized template, convert it to the original non-specialized
  // templated type and use that for the SymbolID.
  // Otherwise clang will consider the specialized type distinct from the non-specialized
  // type and unnecessarily give it a different ID.
  if (const auto* nonspec = getNonSpecializedVersionOfDecl((clang::TagDecl*)ret)) {
    return buildID(nonspec);
  } else if (ret == NULL) {
    return hdoc::types::SymbolID();
  } else {
    return buildID(ret);
  }
}

void hdoc::indexer::matchers::FunctionMatcher::run(const clang::ast_matchers::MatchFinder::MatchResult& Result) {
  const auto res = Result.Nodes.getNodeAs<clang::FunctionDecl>("function");

  // Count the number of functions matched
  this->index->functions.numMatches++;

  // Ignore invalid matches, matches in ignored files, and static functions
  if (res == nullptr || res->isOverloadedOperator() ||
      isInIgnoreList(res, this->cfg->ignorePaths, this->cfg->inputDir) || !res->getSourceRange().isValid() ||
      (res->isStatic() && !res->isCXXClassMember()) || isInAnonymousNamespace(res) ||
      (res->getAccess() == clang::AS_private && cfg->ignorePrivateMembers == true)) {
    return;
  }

  const hdoc::types::SymbolID ID = buildID(res);
  if (this->index->functions.contains(ID)) {
    return;
  }
  this->index->functions.reserve(ID);
  hdoc::types::FunctionSymbol f;
  f.ID = ID;
  fillOutSymbol(f, res, this->cfg->inputDir);

  // Get a bunch of qualifiers
  f.isVariadic   = res->isVariadic();
  f.isVirtual    = res->isVirtualAsWritten();
  f.isConstexpr  = res->isConstexprSpecified() && !res->isExplicitlyDefaulted();
  f.isConsteval  = res->isConsteval();
  f.isInline     = res->isInlineSpecified();
  f.isNoExcept   = clang::isNoexceptExceptionSpec(res->getExceptionSpecType());
  f.storageClass = res->getStorageClass();
  f.access       = res->getAccess();

  // Get ref qualifiers
  if (const auto* fp = res->getType()->getAs<clang::FunctionProtoType>()) {
    f.refQualifier      = fp->getRefQualifier();
    f.hasTrailingReturn = fp->hasTrailingReturn();
  }

  // Add CVR qualifiers
  if (const auto* functionT = res->getType()->getAs<clang::FunctionType>()) {
    f.isConst    = functionT->isConst();
    f.isVolatile = functionT->isVolatile();
    f.isRestrict = functionT->isRestrict();
  }

  // Get arguments and their default values if they exist
  clang::PrintingPolicy pp(res->getASTContext().getLangOpts());
  f.params.reserve(res->param_size());
  for (const auto* i : res->parameters()) {
    hdoc::types::FunctionParam a;
    a.name      = i->getNameAsString();
    a.type.name = i->getType().getAsString(pp);
    a.type.id   = getTypeSymbolID(i->getType());
    if (i->hasDefaultArg()) {
      a.defaultValue = i->hasUninstantiatedDefaultArg() ? exprToString(i->getUninstantiatedDefaultArg(), pp)
                                                        : exprToString(i->getDefaultArg(), pp);
    }
    f.params.emplace_back(a);
  }

  if (clang::FunctionTemplateDecl* templateDecl = res->getDescribedFunctionTemplate()) {
    for (const auto* parameterDecl : *templateDecl->getTemplateParameters()) {
      hdoc::types::TemplateParam tparam;
      if (const auto* templateType = llvm::dyn_cast<clang::TemplateTypeParmDecl>(parameterDecl)) {
        tparam.templateType    = hdoc::types::TemplateParam::TemplateType::TemplateTypeParameter;
        tparam.isParameterPack = templateType->isParameterPack();
        tparam.isTypename      = templateType->wasDeclaredWithTypename();
        tparam.name            = templateType->getNameAsString();
        tparam.defaultValue =
            templateType->hasDefaultArgument() ? templateType->getDefaultArgument().getAsString(pp) : "";
      } else if (const auto* nonTypeTemplate = llvm::dyn_cast<clang::NonTypeTemplateParmDecl>(parameterDecl)) {
        tparam.templateType    = hdoc::types::TemplateParam::TemplateType::NonTypeTemplate;
        tparam.isParameterPack = nonTypeTemplate->isParameterPack();
        tparam.name            = nonTypeTemplate->getNameAsString();
        tparam.defaultValue =
            nonTypeTemplate->hasDefaultArgument() ? exprToString(nonTypeTemplate->getDefaultArgument(), pp) : "";
        tparam.type = nonTypeTemplate->getType().getAsString(pp);
      }
      f.templateParams.emplace_back(tparam);
    }
  }

  const clang::comments::Comment* comment = res->getASTContext().getCommentForDecl(res, nullptr);
  if (comment != nullptr) {
    processSymbolComment(f, comment, res->getASTContext());
  }

  // Don't print "void" return type for constructors and destructors.
  f.isCtorOrDtor = clang::isa<clang::CXXConstructorDecl>(res) || clang::isa<clang::CXXDestructorDecl>(res);
  if (f.isCtorOrDtor == false) {
    f.returnType.name = res->getReturnType().getAsString(pp);
    f.returnType.id   = getTypeSymbolID(res->getReturnType());
  }
  f.proto          = getFunctionSignature(f);
  f.isRecordMember = res->isCXXClassMember();

  findParentNamespace(f, res);
  this->index->functions.update(f.ID, f);
}

void hdoc::indexer::matchers::RecordMatcher::run(const clang::ast_matchers::MatchFinder::MatchResult& Result) {
  const auto res = Result.Nodes.getNodeAs<clang::CXXRecordDecl>("record");

  // Count the number of records matched
  this->index->records.numMatches++;

  // Ignore invalid matches
  if (res == nullptr || !res->isCompleteDefinition() || !res->getSourceRange().isValid() ||
      isInIgnoreList(res, this->cfg->ignorePaths, this->cfg->inputDir) || isInAnonymousNamespace(res)) {
    return;
  }

  // Try to deduce name of structs/unions with C-style typedef decls
  // If the name is empty we try to find the name using some AST hackery, and save the name to be populated later
  std::string cachedName;
  if (res->getNameAsString() == "") {
    if (const auto* possibleTagDecl = llvm::cast_or_null<const clang::TagDecl>(res)) {
      if (const clang::TypedefNameDecl* possibleTypedefDecl = possibleTagDecl->getTypedefNameForAnonDecl()) {
        cachedName = possibleTypedefDecl->getNameAsString();
      }
    }
    if (cachedName == "") {
      return;
    }
  }

  // Skip compiler-injected class specializations not caught by the above (pulled from adobe/hyde)
  if (const auto* s = llvm::dyn_cast_or_null<clang::ClassTemplateSpecializationDecl>(res)) {
    if (!s->getTypeAsWritten()) {
      return;
    }
  }

  const hdoc::types::SymbolID ID = buildID(res);
  if (this->index->records.contains(ID)) {
    return;
  }
  this->index->records.reserve(ID);
  hdoc::types::RecordSymbol c;
  c.ID = ID;
  fillOutSymbol(c, res, this->cfg->inputDir);

  // Apply the cached name found earlier for suspected typedef'ed decls
  if (c.name == "") {
    c.name = cachedName;
  }

  if (const auto* parent = llvm::dyn_cast<clang::CXXRecordDecl>(res->getParent())) {
    c.name = parent->getNameAsString() + "::" + c.name;
  }

  // Get methods and decls (what's the difference?) for this record
  for (const auto* m : res->methods()) {
    if (m == nullptr || m->isImplicit() || m->isOverloadedOperator() ||
        isInIgnoreList(m, this->cfg->ignorePaths, this->cfg->inputDir) || isInAnonymousNamespace(m) ||
        (m->getAccess() == clang::AS_private && cfg->ignorePrivateMembers == true)) {
      continue;
    }
    c.methodIDs.emplace_back(buildID(m->getCanonicalDecl()));
  }
  for (const auto* d : res->decls()) {
    if (const auto* ftd = llvm::dyn_cast<clang::FunctionTemplateDecl>(d)) {
      if (ftd == nullptr || ftd->isImplicit() || ftd->getAsFunction()->isOverloadedOperator() ||
          isInIgnoreList(ftd, this->cfg->ignorePaths, this->cfg->inputDir) || isInAnonymousNamespace(ftd) ||
          (ftd->getAccess() == clang::AS_private && cfg->ignorePrivateMembers == true)) {
        continue;
      }
      c.methodIDs.emplace_back(buildID(ftd));
    }
  }

  // Find records this record inherits from
  if (res->isThisDeclarationADefinition()) {
    for (const auto base : res->bases()) {
      if (const auto* baseRecord = base.getType()->getAsCXXRecordDecl()) {
        // add std prefix for records that are in that namespace
        if (baseRecord->isInStdNamespace()) {
          c.baseRecords.push_back(
              {buildID(baseRecord), base.getAccessSpecifier(), "std::" + baseRecord->getNameAsString()});
        }
        // Records that should be in the DB
        else {
          c.baseRecords.push_back({buildID(baseRecord), base.getAccessSpecifier(), baseRecord->getNameAsString()});
        }
      }
    }
  }

  // Determine record type
  c.type = res->getKindName();

  // Get full declaration including templates
  clang::PrintingPolicy pp(res->getASTContext().getLangOpts());
  if (const auto* templateDecl = res->getDescribedClassTemplate()) {
    for (const auto* paramDecl : *templateDecl->getTemplateParameters()) {
      hdoc::types::TemplateParam tparam;
      if (const auto& templateType = llvm::dyn_cast<clang::TemplateTypeParmDecl>(paramDecl)) {
        tparam.templateType    = hdoc::types::TemplateParam::TemplateType::TemplateTypeParameter;
        tparam.isTypename      = templateType->wasDeclaredWithTypename();
        tparam.isParameterPack = templateType->isParameterPack();
        tparam.name            = templateType->getNameAsString();
        // Get default argument if it exists
        tparam.defaultValue =
            templateType->hasDefaultArgument() ? templateType->getDefaultArgument().getAsString(pp) : "";
      } else if (const auto* nonTypeTemplate = llvm::dyn_cast<clang::NonTypeTemplateParmDecl>(paramDecl)) {
        tparam.templateType    = hdoc::types::TemplateParam::TemplateType::NonTypeTemplate;
        tparam.type            = nonTypeTemplate->getType().getAsString(pp);
        tparam.isParameterPack = nonTypeTemplate->isParameterPack();
        tparam.name            = nonTypeTemplate->getNameAsString();
        // Get default argument if it exists
        tparam.defaultValue =
            nonTypeTemplate->hasDefaultArgument() ? exprToString(nonTypeTemplate->getDefaultArgument(), pp) : "";
      } else if (const auto* templateTemplateType = llvm::dyn_cast<clang::TemplateTemplateParmDecl>(paramDecl)) {
        tparam.templateType = hdoc::types::TemplateParam::TemplateType::TemplateTemplateType;
        tparam.type =
            clang::Lexer::getSourceText(clang::CharSourceRange::getCharRange(templateTemplateType->getSourceRange()),
                                        res->getASTContext().getSourceManager(),
                                        res->getASTContext().getLangOpts());
        tparam.name            = templateTemplateType->getNameAsString();
        tparam.isParameterPack = templateTemplateType->isParameterPack() ? "..." : "";
      }
      c.templateParams.emplace_back(tparam);
    }
  }

  c.proto = getRecordProto(c);

  // TODO: fix this hack
  // If there is an anonymous struct/enum/union declared as a member variable of a record, clang
  // will make its type "enum (anonymous $TYPE at path/to/file)"
  // This is not ideal, and I haven't found a way to elegantly pinpoint this case
  // Consequently, we're using a substring match to see if that string appears in the type string
  // and then we discard decls that match this condition at the call site
  auto isAnonRecordMemberVar = [&](const auto& decl) {
    return decl->getType().getAsString(pp).find("anonymous ") != std::string::npos;
  };

  // TODO: refactor the member variables and static member variable blocks to consolidate duplicated code
  for (const auto* field : res->fields()) {
    if (field->getAccess() == clang::AS_private && cfg->ignorePrivateMembers == true) {
      continue;
    }

    hdoc::types::MemberVariable mv;
    mv.isStatic     = false;
    mv.name         = field->getNameAsString();
    mv.defaultValue = field->hasInClassInitializer() ? exprToString(field->getInClassInitializer(), pp) : "";
    mv.access       = field->getAccess();

    // Ignore anonymous structs and unions that may appear as member variables
    // Anonymous records have their types recorded as "anonymous struct at $FILE:$LINE"
    // which is ugly, so we replace it with out own
    if (field->isAnonymousStructOrUnion() || isAnonRecordMemberVar(field)) {
      mv.type.name = "anonymous struct/union";
    } else {
      mv.type.name = field->getType().getAsString(pp);
      mv.type.id   = getTypeSymbolID(field->getType());
    }

    const clang::comments::Comment* comment = res->getASTContext().getCommentForDecl(field, nullptr);
    if (comment != nullptr) {
      for (auto c = comment->child_begin(); c != comment->child_end(); ++c) {
        if (const auto* paraComment = llvm::dyn_cast<clang::comments::ParagraphComment>(*c)) {
          mv.docComment = getParaCommentContents(paraComment, res->getASTContext());
        }
      }
    }

    c.vars.emplace_back(mv);
  }

  // Get static members that aren't caught by res->fields()
  for (const auto* d : res->decls()) {
    if (const auto* vd = llvm::dyn_cast<clang::VarDecl>(d)) {
      if (vd == nullptr || (vd->getAccess() == clang::AS_private && cfg->ignorePrivateMembers == true)) {
        continue;
      }

      hdoc::types::MemberVariable mv;
      mv.isStatic     = true;
      mv.name         = vd->getNameAsString();
      mv.defaultValue = vd->hasInit() ? exprToString(vd->getInit(), pp) : "";
      mv.access       = vd->getAccess();

      // See previous section for explanation
      if (isAnonRecordMemberVar(vd)) {
        mv.type.name = "anonymous struct/union";
      } else {
        mv.type.name = vd->getType().getAsString(pp);
        mv.type.id   = getTypeSymbolID(vd->getType());
      }

      const clang::comments::Comment* comment = res->getASTContext().getCommentForDecl(vd, nullptr);
      if (comment != nullptr) {
        for (auto c = comment->child_begin(); c != comment->child_end(); ++c) {
          if (const auto* paraComment = llvm::dyn_cast<clang::comments::ParagraphComment>(*c)) {
            mv.docComment = getParaCommentContents(paraComment, res->getASTContext());
          }
        }
      }

      c.vars.emplace_back(mv);
    }
  }

  const clang::comments::Comment* comment = res->getASTContext().getCommentForDecl(res, nullptr);
  if (comment != nullptr) {
    processSymbolComment(c, comment, res->getASTContext());
  }

  findParentNamespace(c, res);
  this->index->records.update(c.ID, c);
}

void hdoc::indexer::matchers::EnumMatcher::run(const clang::ast_matchers::MatchFinder::MatchResult& Result) {
  const auto res = Result.Nodes.getNodeAs<clang::EnumDecl>("enum");

  // Count the number of classes matched
  this->index->enums.numMatches++;

  // Ignore invalid matches and anonymous enums
  if (res == nullptr || res->getNameAsString() == "" ||
      isInIgnoreList(res, this->cfg->ignorePaths, this->cfg->inputDir) || isInAnonymousNamespace(res)) {
    return;
  }

  const hdoc::types::SymbolID ID = buildID(res);
  if (this->index->enums.contains(ID)) {
    return;
  }
  this->index->enums.reserve(ID);
  hdoc::types::EnumSymbol e;
  e.ID = ID;
  fillOutSymbol(e, res, this->cfg->inputDir);

  if (const auto* parent = llvm::dyn_cast<clang::CXXRecordDecl>(res->getParent())) {
    e.name = parent->getNameAsString() + "::" + e.name;
  }

  // Determine if this enum is scoped, i.e. an "enum class" or "enum struct"
  if (res->isScoped()) {
    if (res->isScopedUsingClassTag()) {
      e.type = "enum class";
    } else {
      e.type = "enum struct";
    }
  } else {
    e.type = "enum";
  }

  for (const auto* m : res->enumerators()) {
    hdoc::types::EnumMember em;
    em.name  = m->getNameAsString();
    em.value = m->getInitVal().getExtValue();

    // Grab comment for this enum value
    const auto memberComment = res->getASTContext().getCommentForDecl(m, nullptr);
    if (memberComment != nullptr) {
      for (auto c = memberComment->child_begin(); c != memberComment->child_end(); ++c) {
        if (const auto* paraComment = llvm::dyn_cast<clang::comments::ParagraphComment>(*c)) {
          em.docComment += getParaCommentContents(paraComment, res->getASTContext());
        }
      }
    }
    e.members.emplace_back(em);
  }

  const clang::comments::Comment* comment = res->getASTContext().getCommentForDecl(res, nullptr);
  if (comment != nullptr) {
    processSymbolComment(e, comment, res->getASTContext());
  }

  findParentNamespace(e, res);
  this->index->enums.update(e.ID, e);
}

void hdoc::indexer::matchers::NamespaceMatcher::run(const clang::ast_matchers::MatchFinder::MatchResult& Result) {
  const auto res = Result.Nodes.getNodeAs<clang::NamespaceDecl>("namespace");

  // Count the number of namespaces matched
  this->index->namespaces.numMatches++;

  // Ignore invalid matches and anonymous enums
  if (res == nullptr || res->getNameAsString() == "" ||
      isInIgnoreList(res, this->cfg->ignorePaths, this->cfg->inputDir) || isInAnonymousNamespace(res)) {
    return;
  }

  const hdoc::types::SymbolID ID = buildID(res);
  if (this->index->namespaces.contains(ID)) {
    return;
  }
  this->index->namespaces.reserve(ID);
  hdoc::types::NamespaceSymbol n;
  n.ID = ID;
  fillOutSymbol(n, res, this->cfg->inputDir);

  findParentNamespace(n, res);
  this->index->namespaces.update(n.ID, n);
}
