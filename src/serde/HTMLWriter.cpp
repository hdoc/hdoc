// Copyright 2019-2022 hdoc
// SPDX-License-Identifier: AGPL-3.0-only

#include "ctml.hpp"
#include "spdlog/spdlog.h"
#include "clang/Basic/Specifiers.h"
#include "clang/Format/Format.h"
#include "llvm/Support/JSON.h"

#include <filesystem>
#include <fstream>
#include <stack>
#include <string>

#include "serde/CppReferenceURLs.hpp"
#include "serde/HTMLWriter.hpp"
#include "support/MarkdownConverter.hpp"
#include "support/StringUtils.hpp"
#include "types/Symbols.hpp"

/// Implementation of to_string() for Clang member variable access specifier
static std::string to_string(const clang::AccessSpecifier& access) {
  switch (access) {
  case clang::AccessSpecifier::AS_public:
    return "public";
  case clang::AccessSpecifier::AS_protected:
    return "protected";
  case clang::AccessSpecifier::AS_private:
    return "private";
  case clang::AccessSpecifier::AS_none:
    return "none";
  default:
    return "unknown";
  }
}

/// Returns a vector of all SymbolIDs in a given database
/// Useful for getting a vector of SymbolIDs that will be sorted
template <typename T> static std::vector<hdoc::types::SymbolID> map2vec(const hdoc::types::Database<T>& db) {
  std::vector<hdoc::types::SymbolID> IDs;
  IDs.reserve(db.entries.size());
  for (const auto& [k, v] : db.entries) {
    IDs.push_back(k);
  }
  return IDs;
}

/// Sort a vector of SymbolIDs alphabetically by the name of the Symbol they point to
/// Note: all members of IDs need to be of type T
template <typename T>
static std::vector<hdoc::types::SymbolID> getSortedIDs(const std::vector<hdoc::types::SymbolID>& IDs,
                                                       const hdoc::types::Database<T>&           db) {
  std::vector<T> symbols = {};
  symbols.reserve(IDs.size());
  for (const auto& id : IDs) {
    symbols.push_back(db.entries.at(id));
  }
  std::sort(symbols.begin(), symbols.end());
  std::vector<hdoc::types::SymbolID> sortedIDs;
  sortedIDs.reserve(IDs.size());
  for (const auto& s : symbols) {
    sortedIDs.push_back(s.ID);
  }
  return sortedIDs;
}

extern uint8_t      ___assets_styles_css[];
extern uint8_t      ___assets_favicon_ico[];
extern uint8_t      ___assets_favicon_32x32_png[];
extern uint8_t      ___assets_favicon_16x16_png[];
extern uint8_t      ___assets_apple_touch_icon_png[];
extern uint8_t      ___assets_search_js[];
extern uint8_t      ___assets_worker_js[];
extern unsigned int ___assets_styles_css_len;
extern unsigned int ___assets_favicon_ico_len;
extern unsigned int ___assets_favicon_32x32_png_len;
extern unsigned int ___assets_favicon_16x16_png_len;
extern unsigned int ___assets_apple_touch_icon_png_len;
extern unsigned int ___assets_search_js_len;
extern unsigned int ___assets_worker_js_len;

hdoc::serde::HTMLWriter::HTMLWriter(const hdoc::types::Index*  index,
                                    const hdoc::types::Config* cfg,
                                    llvm::ThreadPool&          pool)
    : index(index), cfg(cfg), pool(pool) {
  // Create the directory where the HTML files will be placed
  std::error_code ec;
  if (std::filesystem::exists(this->cfg->outputDir) == false) {
    if (std::filesystem::create_directories(this->cfg->outputDir, ec) == false) {
      spdlog::error(
          "Creation of directory {} failed with error message {}", this->cfg->outputDir.string(), ec.message());
    }
  }

  // hdoc bundles assets (favicons, CSS) with the executable to simplify deployment.
  // The following code collects the files (converted to char arrays in the build process)
  // and outputs them. The process looks janky but it's simple and it works.
  struct BundledFile {
    const unsigned int          len;
    const uint8_t*              file;
    const std::filesystem::path path;
  };

  std::vector<BundledFile> bundledFiles = {
      {___assets_apple_touch_icon_png_len, ___assets_apple_touch_icon_png, cfg->outputDir / "apple-touch-icon.png"},
      {___assets_favicon_16x16_png_len, ___assets_favicon_16x16_png, cfg->outputDir / "favicon-16x16.png"},
      {___assets_favicon_32x32_png_len, ___assets_favicon_32x32_png, cfg->outputDir / "favicon-32x32.png"},
      {___assets_favicon_ico_len, ___assets_favicon_ico, cfg->outputDir / "favicon.ico"},
      {___assets_styles_css_len, ___assets_styles_css, cfg->outputDir / "styles.css"},
      {___assets_search_js_len, ___assets_search_js, cfg->outputDir / "search.js"},
      {___assets_worker_js_len, ___assets_worker_js, cfg->outputDir / "worker.js"}};

  for (const auto& file : bundledFiles) {
    std::ofstream out(file.path, std::ios::binary);
    out.write((char*)file.file, file.len);
    out.close();
  }
}

/// Create a new HTML page with standard structure
/// Optional sidebar, CSS styling, favicons, footer, etc.
static void printNewPage(const hdoc::types::Config&   cfg,
                         CTML::Node                   main,
                         const std::filesystem::path& path,
                         const std::string&           pageTitle,
                         CTML::Node                   breadcrumbs = CTML::Node()) {
  CTML::Document html;

  // Create the header, which includes Bulma CSS framework
  html.AppendNodeToHead(CTML::Node("meta").SetAttr("charset", "utf-8"));
  html.AppendNodeToHead(
      CTML::Node("meta").SetAttr("name", "viewport").SetAttr("content", "width=device-width, initial-scale=1"));
  html.AppendNodeToHead(CTML::Node("title", pageTitle));

  // Use our custom css which is a modified version of bulma
  html.AppendNodeToHead(CTML::Node("link").SetAttr("rel", "stylesheet").SetAttr("href", "styles.css"));

  // highlight.js stylesheet and scripts
  html.AppendNodeToHead(
      CTML::Node("link")
          .SetAttr("rel", "stylesheet")
          .SetAttr("href", "//cdnjs.cloudflare.com/ajax/libs/highlight.js/9.18.1/styles/foundation.min.css"));
  html.AppendNodeToHead(
      CTML::Node("script").SetAttr("src", "//cdnjs.cloudflare.com/ajax/libs/highlight.js/9.18.1/highlight.min.js"));
  html.AppendNodeToHead(CTML::Node("script", "hljs.initHighlightingOnLoad();"));

  // KaTeX configuration
  html.AppendNodeToHead(CTML::Node("link")
                            .SetAttr("rel", "stylesheet")
                            .SetAttr("href", "//cdn.jsdelivr.net/npm/katex@0.13.11/dist/katex.min.css"));
  html.AppendNodeToHead(CTML::Node("script").SetAttr("src", "//cdn.jsdelivr.net/npm/katex@0.13.11/dist/katex.min.js"));
  html.AppendNodeToHead(
      CTML::Node("script").SetAttr("src", "//cdn.jsdelivr.net/npm/katex@0.13.11/dist/contrib/auto-render.min.js"));
  const char* katexConfiguration = R"(
    document.addEventListener("DOMContentLoaded", function() {
      renderMathInElement(document.body, {
        delimiters: [
          {left: '$$', right: '$$', display: true},
          {left: '$', right: '$', display: false},
        ],
      });
    });
  )";
  html.AppendNodeToHead(CTML::Node("script").AppendRawHTML(katexConfiguration));

  // Favicons
  html.AppendNodeToHead(CTML::Node("link")
                            .SetAttr("rel", "apple-touch-icon")
                            .SetAttr("sizes", "180x180")
                            .SetAttr("href", "apple-touch-icon.png"));
  html.AppendNodeToHead(CTML::Node("link")
                            .SetAttr("rel", "icon")
                            .SetAttr("type", "image/png")
                            .SetAttr("sizes", "32x32")
                            .SetAttr("href", "favicon-32x32.png"));
  html.AppendNodeToHead(CTML::Node("link")
                            .SetAttr("rel", "icon")
                            .SetAttr("type", "image/png")
                            .SetAttr("sizes", "16x16")
                            .SetAttr("href", "favicon-16x16.png"));

  CTML::Node wrapperDiv   = CTML::Node("div#wrapper");
  CTML::Node section      = CTML::Node("section.section");
  CTML::Node containerDiv = CTML::Node("div.container");

  // Create a sidebar with navigation links etc
  auto columnsDiv = CTML::Node("div.columns");
  auto mainColumn = CTML::Node("div.column").SetAttr("style", "overflow-x: auto");
  auto aside      = CTML::Node("aside.column is-one-fifth");
  auto menuUL     = CTML::Node("ul.menu-list");

  aside.AddChild(CTML::Node("a.is-button is-size-1", "hdoc").SetAttr("href", "https://hdoc.io"));
  menuUL.AddChild(CTML::Node("p.is-size-4", cfg.projectName + " " + cfg.projectVersion));
  menuUL.AddChild(CTML::Node("p.menu-label", "Navigation"));
  menuUL.AddChild(CTML::Node("li").AddChild(CTML::Node("a", "Home").SetAttr("href", "index.html")));
  menuUL.AddChild(CTML::Node("li").AddChild(CTML::Node("a", "Search").SetAttr("href", "search.html")));
  menuUL.AddChild(CTML::Node("p.menu-label", "API Documentation"));
  menuUL.AddChild(CTML::Node("li").AddChild(CTML::Node("a", "Functions").SetAttr("href", "functions.html")));
  menuUL.AddChild(CTML::Node("li").AddChild(CTML::Node("a", "Records").SetAttr("href", "records.html")));
  menuUL.AddChild(CTML::Node("li").AddChild(CTML::Node("a", "Enums").SetAttr("href", "enums.html")));
  menuUL.AddChild(CTML::Node("li").AddChild(CTML::Node("a", "Namespaces").SetAttr("href", "namespaces.html")));
  // Add paths to markdown pages converted to HTML, if any were provided
  if (cfg.mdPaths.size() > 0) {
    menuUL.AddChild(CTML::Node("p.menu-label", "Pages"));
    for (const auto& f : cfg.mdPaths) {
      std::string path = "doc" + f.filename().replace_extension("html").string();
      std::string name = f.filename().stem().string();
      menuUL.AddChild(CTML::Node("li").AddChild(CTML::Node("a", name).SetAttr("href", path)));
    }
  }
  aside.AddChild(menuUL);

  columnsDiv.AddChild(aside);
  columnsDiv.AddChild(mainColumn.AddChild(breadcrumbs).AddChild(main.SetAttr("class", "content")));
  containerDiv.AddChild(columnsDiv);
  section.AddChild(containerDiv);
  wrapperDiv.AddChild(section);
  html.AppendNodeToBody(wrapperDiv);

  // Create footer with creation date and details
  CTML::Node p1 = CTML::Node("p", "Documentation for " + cfg.projectName + " " + cfg.projectVersion + ".");
  CTML::Node p2 = CTML::Node("p", "Generated by ")
                      .AddChild(CTML::Node("a", "hdoc").SetAttr("href", "https://hdoc.io/"))
                      .AppendText(" version " + cfg.hdocVersion + " on " + cfg.timestamp + ".");
  CTML::Node p3 = CTML::Node("p.has-text-grey-light", "19AD43E11B2996");
  html.AppendNodeToBody(CTML::Node("footer.footer").AddChild(p1).AddChild(p2).AddChild(p3));

  // Dump to a file
  std::ofstream(path) << html.ToString();
}

/// Return a short string describing a symbol for its entry in the overview list
/// If the string contains display math we automatically reject it since it will ruin the formatting
static std::string getSymbolBlurb(const hdoc::types::Symbol& s) {
  // TODO: this is not the most efficient way to write this...
  std::string ret = "";
  if (s.docComment != "") {
    if (s.docComment.size() > 64) {
      ret = " - " + s.docComment.substr(0, 63) + "...";
    } else {
      ret = " - " + s.docComment;
    }
  }

  if (s.briefComment != "") {
    ret = " - " + s.briefComment;
  }

  if (ret.find("$$") != std::string::npos) {
    return "";
  } else {
    return ret;
  }
}

/// Run clang-format with a custom style over the given string
std::string hdoc::serde::clangFormat(const std::string& s, const uint64_t& columnLimit) {
  // Run clang-format over function name to break width to 50 chars
  auto style              = clang::format::getChromiumStyle(clang::format::FormatStyle::LK_Cpp);
  style.ColumnLimit       = columnLimit;
  style.BreakBeforeBraces = clang::format::FormatStyle::BS_Attach;
  auto formattedName =
      clang::tooling::applyAllReplacements(s, clang::format::reformat(style, s, {clang::tooling::Range(0, s.size())}));

  return formattedName.get();
}

/// Returns the "bare" type name (i.e. type name with no qualifiers, pointers, or references)
/// for a given type name.
/// For example, and input of `const Type<int> **` becomes `Type`
std::string hdoc::serde::getBareTypeName(const std::string& typeName) {
  std::string str = typeName;

  // Strip away type qualifiers
  hdoc::utils::replaceFirst(str, "const ", "");
  hdoc::utils::replaceFirst(str, "volatile ", "");
  hdoc::utils::replaceFirst(str, "restrict ", "");
  hdoc::utils::replaceFirst(str, "struct ", "");
  hdoc::utils::replaceFirst(str, "union ", "");

  str = str.substr(0, str.find("<"));
  str = str.substr(0, str.find("&"));
  str = str.substr(0, str.find("*"));
  str = str.substr(0, str.find("("));
  str = str.substr(0, str.find("["));
  hdoc::utils::rtrim(str);

  return str;
}

/// Replaces type names in a function proto with hyperlinked references to
/// those types. Works for indexed records and std:: types found in the map above.
std::string hdoc::serde::getHyperlinkedFunctionProto(const std::string& proto, const hdoc::types::FunctionSymbol& f) {
  std::string str = proto;

  // Replace all of the html-sensitive characters with
  str = hdoc::utils::replaceAll(str, "&", "&amp;");
  str = hdoc::utils::replaceAll(str, "<", "&lt;");
  str = hdoc::utils::replaceAll(str, ">", "&gt;");
  str = hdoc::utils::replaceAll(str, "\"", "&quot;");
  str = hdoc::utils::replaceAll(str, "'", "&apos;");

  std::size_t index              = 0;
  std::string bareReturnTypeName = getBareTypeName(f.returnType.name);
  if (f.returnType.id.hashValue != 0) {
    std::string replacement = "<a href=\"r" + f.returnType.id.str() + ".html\">" + bareReturnTypeName + "</a>";
    index                   = hdoc::utils::replaceFirst(str, bareReturnTypeName, replacement, index);
  }

  if (bareReturnTypeName.substr(0, 5) == "std::") {
    if (StdTypeURLMap.find(bareReturnTypeName) != StdTypeURLMap.end()) {
      std::string replacement = "<a href=\"" + std::string(cppreferenceURL) + StdTypeURLMap.at(bareReturnTypeName) +
                                "\">" + bareReturnTypeName + "</a>";
      index = hdoc::utils::replaceFirst(str, bareReturnTypeName, replacement, index);
    }
  }

  for (const auto& param : f.params) {
    std::string bareParamTypeName = getBareTypeName(param.type.name);
    if (param.type.id.hashValue != 0) {
      std::string replacement = "<a href=\"r" + param.type.id.str() + ".html\">" + bareParamTypeName + "</a>";
      index                   = hdoc::utils::replaceFirst(str, bareParamTypeName, replacement, index);
    }

    if (bareParamTypeName.substr(0, 5) == "std::") {
      if (StdTypeURLMap.find(bareParamTypeName) != StdTypeURLMap.end()) {
        std::string replacement = "<a href=\"" + std::string(cppreferenceURL) + StdTypeURLMap.at(bareParamTypeName) +
                                  "\">" + bareParamTypeName + "</a>";
        index = hdoc::utils::replaceFirst(str, bareParamTypeName, replacement, index);
      }
    }
  }

  return str;
}

/// Returns the typename as raw HTML with hyperlinks where possible.
/// Indexed types are hyperlinked to, as are certain std:: types.
/// All others are returned without hyperlinks as the plain type name.
static std::string getHyperlinkedTypeName(const hdoc::types::TypeRef& type) {
  std::string fullTypeName = type.name;
  std::string bareTypeName = hdoc::serde::getBareTypeName(fullTypeName);

  fullTypeName = hdoc::serde::clangFormat(fullTypeName);
  fullTypeName = hdoc::utils::replaceAll(fullTypeName, "&", "&amp;");
  fullTypeName = hdoc::utils::replaceAll(fullTypeName, "<", "&lt;");
  fullTypeName = hdoc::utils::replaceAll(fullTypeName, ">", "&gt;");
  fullTypeName = hdoc::utils::replaceAll(fullTypeName, "\"", "&quot;");
  fullTypeName = hdoc::utils::replaceAll(fullTypeName, "'", "&apos;");

  if (type.id.raw() == 0) {
    // If it's a std:: type, then try to link to its cppreference page.
    if (bareTypeName.substr(0, 5) == "std::" && StdTypeURLMap.find(bareTypeName) != StdTypeURLMap.end()) {
      std::string replacement =
          "<a href=\"" + std::string(cppreferenceURL) + StdTypeURLMap.at(bareTypeName) + "\">" + bareTypeName + "</a>";
      hdoc::utils::replaceFirst(fullTypeName, bareTypeName, replacement);

      return fullTypeName;
    }
    // Otherwise, return it as-is.
    else {
      return fullTypeName;
    }
  }
  // The type is in the database, so we link to it.
  else {
    std::string replacement = "<a href=\"r" + type.id.str() + ".html\">" + bareTypeName + "</a>";
    hdoc::utils::replaceFirst(fullTypeName, bareTypeName, replacement);
    return fullTypeName;
  }
}

/// Returns an HTML node indicating where the s is declared.
/// A hyperlink to the exact line in the source file (for GitHub and GitLab) is returned
/// if gitRepoURL is provided.
static CTML::Node getDeclaredAtNode(const hdoc::types::Symbol& s, const std::string& gitRepoURL = "") {
  auto p = CTML::Node("p", "Declared at: ");
  if (gitRepoURL == "") {
    return p.AddChild(CTML::Node("span.is-family-code", s.file + ":" + std::to_string(s.line)));
  } else {
    return p.AddChild(CTML::Node("a.is-family-code", s.file + ":" + std::to_string(s.line))
                          .SetAttr("href", gitRepoURL + s.file + "#L" + std::to_string(s.line)));
  }
}

/// Creates a Bulma breadcrumb node to make the provenance of the current symbol more clear and aid in navigation.
static CTML::Node
getBreadcrumbNode(const std::string& prefix, const hdoc::types::Symbol& s, const hdoc::types::Index& index) {
  // Symbols that have no parents don't have any breadcrumbs.
  if (s.parentNamespaceID.raw() == 0) {
    return CTML::Node();
  }

  auto nav = CTML::Node("nav.breadcrumb has-arrow-separator").SetAttr("aria-label", "breadcrumbs");
  auto ul  = CTML::Node("ul");

  struct ParentSymbol {
    std::string         symbolType;
    hdoc::types::Symbol symbol;
  };

  // Construct a LIFO stack of parents for the current symbol.
  // LIFO is used because we need to print the nodes into HTML in reverse order.
  std::stack<ParentSymbol> stack;
  hdoc::types::Symbol      parent = s;
  while (true) {
    if (index.namespaces.contains(parent.parentNamespaceID)) {
      const auto& newParent = index.namespaces.entries.at(parent.parentNamespaceID);
      stack.push({"namespace", newParent});
      parent = newParent;
    } else if (index.records.contains(parent.parentNamespaceID)) {
      const auto& newParent = index.records.entries.at(parent.parentNamespaceID);
      stack.push({newParent.type, newParent});
      parent = newParent;
    } else {
      break;
    }
  }

  // Create the HTML nodes for the parent symbols of the current node.
  while (!stack.empty()) {
    const auto parent = stack.top();
    stack.pop();

    auto li = CTML::Node("li");
    auto a  = CTML::Node();
    if (parent.symbolType == "namespace") {
      a = CTML::Node("a").SetAttr("href", "namespaces.html#" + parent.symbol.ID.str());
    } else {
      a = CTML::Node("a").SetAttr("href", "r" + parent.symbol.ID.str() + ".html");
    }
    auto span = CTML::Node("span", parent.symbolType + " " + parent.symbol.name);

    ul.AddChild(li.AddChild(a.AddChild(span)));
  }

  // Add the final breadcrumb, which is the actual symbol itself.
  auto li   = CTML::Node("li.is-active");
  auto a    = CTML::Node("a").SetAttr("aria-current", "page" + s.ID.str());
  auto span = CTML::Node("span", prefix + " " + s.name);
  ul.AddChild(li.AddChild(a.AddChild(span)));

  return nav.AddChild(ul);
}

/// Print a function to main
static void printFunction(const hdoc::types::FunctionSymbol& f, CTML::Node& main, const std::string& gitRepoURL) {
  // Print function return type, name, and parameters as section header
  std::string proto = hdoc::serde::getHyperlinkedFunctionProto(hdoc::serde::clangFormat(f.proto), f);
  auto        inner = CTML::Node("code.language-cpp").AppendRawHTML(proto);
  main.AddChild(CTML::Node("h3#" + f.ID.str()).AddChild(CTML::Node("pre").AddChild(inner)));

  // Print function description only if there's an associated comment
  if (f.briefComment != "" || f.docComment != "") {
    main.AddChild(CTML::Node("h4", "Description"));
  }

  if (f.briefComment != "") {
    main.AddChild(CTML::Node("p", f.briefComment));
  }
  if (f.docComment != "") {
    main.AddChild(CTML::Node("p", f.docComment));
  }
  main.AddChild(getDeclaredAtNode(f, gitRepoURL));

  // Print function parameters (with type, name, default value, and comment) as a list
  if (f.params.size() > 0) {
    main.AddChild(CTML::Node("h4", "Parameters"));
    CTML::Node dl("dl");

    for (auto param : f.params) {
      auto dt = CTML::Node("dt.is-family-code").AppendRawHTML(getHyperlinkedTypeName(param.type));
      dt.AddChild(CTML::Node("b", " " + param.name));

      if (param.defaultValue != "") {
        dt.AppendText(" = " + param.defaultValue);
      }
      dl.AddChild(dt);
      if (param.docComment != "") {
        dl.AddChild(CTML::Node("dd", param.docComment));
      }
    }
    main.AddChild(dl);
  }

  // Return value description
  if (f.returnTypeDocComment != "") {
    main.AddChild(CTML::Node("h4", "Returns"));
    main.AddChild(CTML::Node("p", f.returnTypeDocComment));
  }
}

/// Print all of the functions that aren't record members in a project
void hdoc::serde::HTMLWriter::printFunctions() const {
  CTML::Node main("main");
  main.AddChild(CTML::Node("h1", "Functions"));

  // Print a bullet list of functions
  uint64_t   numFunctions = 0; // Number of functions that aren't methods
  CTML::Node ul("ul");
  for (const auto& id : getSortedIDs(map2vec(this->index->functions), this->index->functions)) {
    const auto& f = this->index->functions.entries.at(id);
    if (f.isRecordMember) {
      continue;
    }
    numFunctions += 1;
    ul.AddChild(CTML::Node("li")
                    .AddChild(CTML::Node("a.is-family-code", f.name).SetAttr("href", f.url()))
                    .AppendText(getSymbolBlurb(f)));
    CTML::Node page("main");
    this->pool.async(
        [&](const hdoc::types::FunctionSymbol& func, CTML::Node pg) {
          printFunction(func, pg, this->cfg->gitRepoURL);
          printNewPage(*this->cfg,
                       pg,
                       this->cfg->outputDir / func.url(),
                       "function " + func.name + ": " + this->cfg->projectName + " " + this->cfg->projectVersion +
                           " documentation",
                       getBreadcrumbNode("function", func, *this->index));
        },
        f,
        page);
  }
  this->pool.wait();
  main.AddChild(CTML::Node("h2", "Overview"));
  if (numFunctions == 0) {
    main.AddChild(CTML::Node("p", "No functions were declared in this project."));
  } else {
    main.AddChild(ul);
  }
  printNewPage(*this->cfg,
               main,
               this->cfg->outputDir / "functions.html",
               "Functions: " + this->cfg->projectName + " " + this->cfg->projectVersion + " documentation");
}

static std::vector<hdoc::types::RecordSymbol::BaseRecord> getInheritedSymbols(const hdoc::types::Index*        index,
                                                                              const hdoc::types::RecordSymbol& root) {
  std::vector<hdoc::types::RecordSymbol::BaseRecord> vec   = {};
  std::stack<hdoc::types::RecordSymbol::BaseRecord>  stack = {};
  for (const auto& base : root.baseRecords) {
    stack.push(base);
  }

  // Do a depth-first traversal of the parents of root
  while (!stack.empty()) {
    const hdoc::types::RecordSymbol::BaseRecord record = stack.top();
    stack.pop();

    // Quit if the base record is in std namespace
    if (index->records.contains(record.id) == false) {
      continue;
    }

    // Records inherited privately are ignored and their children are not traversed
    // This is suboptimal since an immediate privately inherited parent of root might have some important members
    // we'd like to document; for now I'm not implementing that since that edge case would balloon code complexity
    if (record.access == clang::AS_private) {
      continue;
    }

    vec.push_back(record);

    // Add children to stack for traversing
    const auto& c = index->records.entries.at(record.id);
    for (const auto& baseRecord : c.baseRecords) {
      stack.push(baseRecord);
    }
  }
  return vec;
}

static void printMemberVariables(const hdoc::types::RecordSymbol& c, CTML::Node& main, const bool& isInherited) {
  CTML::Node dl("dl");
  uint64_t   numVars = 0;

  for (const hdoc::types::MemberVariable& var : c.vars) {
    if (isInherited == true && var.access == clang::AS_private) {
      continue;
    }

    std::string preamble = to_string(var.access);
    preamble += var.isStatic ? " static " : " ";

    CTML::Node dt;
    // Print the access, type, name, and doc comment if it exists
    if (isInherited == false) {
      dt     = CTML::Node("dt.is-family-code").AppendRawHTML(preamble + " " + getHyperlinkedTypeName(var.type) + " ");
      auto b = CTML::Node("b", var.name);
      dt.AddChild(b);
      dt.SetAttr("id", "var_" + var.name);
    }
    // Inherited variables get a bullet point and link to the description in the parent record
    else {
      dt = CTML::Node("dt.is-family-code");
      const auto a =
          CTML::Node("a", preamble).SetAttr("href", c.url() + "#var_" + var.name).AddChild(CTML::Node("b", var.name));
      dt.AddChild(a);
    }
    if (var.defaultValue != "") {
      dt.AppendText(" = " + var.defaultValue);
    }

    dl.AddChild(dt);

    if (isInherited == false && var.docComment != "") {
      dl.AddChild(CTML::Node("dd", var.docComment));
    }

    numVars += 1;
  }

  if (numVars > 0) {
    if (isInherited) {
      main.AddChild(CTML::Node("p", "Inherited from ")
                        .AddChild(CTML::Node("a", c.name).SetAttr("href", c.url()))
                        .AppendText(":"));
    }
    main.AddChild(dl);
  }
}

/// Print a list of inherited methods for the given record, truncating the method declaration
static void
printInheritedMethods(const hdoc::types::Index* index, const hdoc::types::RecordSymbol& c, CTML::Node& main) {
  auto ul = CTML::Node("ul");

  for (const auto& methodID : getSortedIDs(c.methodIDs, index->functions)) {
    const auto& f = index->functions.entries.at(methodID);
    // Skip private functions and ctors/dtors that aren't inherited
    if (f.access == clang::AS_private || f.isCtorOrDtor) {
      continue;
    }

    const auto li = CTML::Node("li.is-family-code")
                        .AddChild(CTML::Node("a", to_string(f.access) + " ")
                                      .SetAttr("href", c.url() + "#" + f.ID.str())
                                      .AddChild(CTML::Node("b", f.name)));
    ul.AddChild(li);
  }

  if (c.methodIDs.size() > 0) {
    main.AddChild(
        CTML::Node("p", "Inherited from ").AddChild(CTML::Node("a", c.name).SetAttr("href", c.url())).AppendText(":"));
    main.AddChild(ul);
  }
}

/// Print a record to main
void hdoc::serde::HTMLWriter::printRecord(const hdoc::types::RecordSymbol& c) const {
  CTML::Node main("main");

  const std::string pageTitle = c.type + " " + c.name;
  main.AddChild(CTML::Node("h1", pageTitle));

  // Full declaration
  main.AddChild(CTML::Node("h2", "Declaration"));
  main.AddChild(CTML::Node("pre").AddChild(
      CTML::Node("code.language-cpp", hdoc::serde::clangFormat(c.proto, 70) + " { /* full declaration omitted */ };")));

  if (c.briefComment != "" || c.docComment != "") {
    main.AddChild(CTML::Node("h2", "Description"));
  }
  if (c.briefComment != "") {
    main.AddChild(CTML::Node("p", c.briefComment));
  }
  if (c.docComment != "") {
    main.AddChild(CTML::Node("p", c.docComment));
  }
  main.AddChild(getDeclaredAtNode(c, this->cfg->gitRepoURL));

  // Base records
  uint64_t count = 0;
  if (c.baseRecords.size() > 0) {
    auto baseP = CTML::Node("p", "Inherits from: ");
    for (const auto& baseRecord : c.baseRecords) {
      if (count > 0) {
        baseP.AppendText(", ");
      }
      // Check if type is a string, indicating it's a std record that isn't in the DB
      if (this->index->records.contains(baseRecord.id) == false) {
        baseP.AppendText(baseRecord.name);
      } else {
        const auto& p = this->index->records.entries.at(baseRecord.id);
        baseP.AddChild(CTML::Node("a", p.name).SetAttr("href", p.url()));
      }
      count++;
    }
    main.AddChild(baseP);
  }

  // Print regular member variables
  bool hasMemberVariableHeading = false;
  if (c.vars.size() > 0) {
    main.AddChild(CTML::Node("h2", "Member Variables"));
    hasMemberVariableHeading = true;
    printMemberVariables(c, main, false);
  }

  // Print inherited member variables
  const auto inheritedRecords = getInheritedSymbols(this->index, c);
  for (const auto& base : inheritedRecords) {
    const auto& ic = this->index->records.entries.at(base.id);
    if (hasMemberVariableHeading == false && ic.vars.size() > 0) {
      main.AddChild(CTML::Node("h2", "Member Variables"));
      hasMemberVariableHeading = true;
    }
    printMemberVariables(ic, main, true);
  }

  // Method overview in list form
  const auto& sortedMethodIDs          = getSortedIDs(c.methodIDs, this->index->functions);
  bool        hasMethodOverviewHeading = false;
  if (sortedMethodIDs.size() > 0) {
    main.AddChild(CTML::Node("h2", "Method Overview"));
    hasMethodOverviewHeading = true;
    CTML::Node ul("ul");
    for (auto methodID : sortedMethodIDs) {
      const hdoc::types::FunctionSymbol m = this->index->functions.entries.at(methodID);

      // Divide up the full function declaration so its name can be bold in the HTML
      const uint64_t    nameLen  = m.name.size();
      const std::string preName  = to_string(m.access) + " " + m.proto.substr(0, m.nameStart) + " ";
      const std::string postName = m.proto.substr(m.nameStart + nameLen, m.proto.size() - m.nameStart - nameLen);

      const auto li = CTML::Node("li.is-family-code")
                          .AddChild(CTML::Node("a", preName)
                                        .SetAttr("href", "#" + m.ID.str())
                                        .AddChild(CTML::Node("b", m.name))
                                        .AppendText(postName));
      ul.AddChild(li);
    }
    main.AddChild(ul);
  }

  // Add inherited methods to the list
  for (const auto& base : inheritedRecords) {
    const auto& ic = this->index->records.entries.at(base.id);
    if (hasMethodOverviewHeading == false && c.methodIDs.size() > 0) {
      main.AddChild(CTML::Node("h2", "Method Overview"));
      hasMethodOverviewHeading = true;
    }
    printInheritedMethods(this->index, ic, main);
  }

  // List of methods with full information
  if (sortedMethodIDs.size() > 0) {
    main.AddChild(CTML::Node("h2", "Methods"));
    for (const auto& methodID : sortedMethodIDs) {
      // TODO: get to the bottom of what's causing empty method decls to appear in Writer.hpp
      // For now this hack just avoids printing them, but this shouldn't be necessary
      if (index->functions.contains(methodID) == false) {
        continue;
      }
      printFunction(this->index->functions.entries.at(methodID), main, this->cfg->gitRepoURL);
    }
  }

  printNewPage(*this->cfg,
               main,
               this->cfg->outputDir / c.url(),
               pageTitle + ": " + this->cfg->projectName + " " + this->cfg->projectVersion + " documentation",
               getBreadcrumbNode(c.type, c, *this->index));
}

/// Print all of the records in a project
void hdoc::serde::HTMLWriter::printRecords() const {
  CTML::Node main("main");
  main.AddChild(CTML::Node("h1", "Records"));

  // List of all the records defined, with links to the individual record HTML
  CTML::Node ul("ul");
  for (const auto& id : getSortedIDs(map2vec(this->index->records), this->index->records)) {
    const auto& c = this->index->records.entries.at(id);
    ul.AddChild(CTML::Node("li")
                    .AddChild(CTML::Node("a.is-family-code", c.type + " " + c.name).SetAttr("href", c.url()))
                    .AppendText(getSymbolBlurb(c)));
    this->pool.async([&](const hdoc::types::RecordSymbol& cls) { printRecord(cls); }, c);
  }
  this->pool.wait();
  main.AddChild(CTML::Node("h2", "Overview"));
  if (this->index->records.entries.size() == 0) {
    main.AddChild(CTML::Node("p", "No records were declared in this project."));
  } else {
    main.AddChild(ul);
  }
  printNewPage(*this->cfg,
               main,
               this->cfg->outputDir / "records.html",
               "Records: " + this->cfg->projectName + " " + this->cfg->projectVersion + " documentation");
}

/// Recursively print an single namespace and all of its children
/// Should be tail-call optimized
static CTML::Node printNamespace(const hdoc::types::NamespaceSymbol& ns, const hdoc::types::Index& index) {
  // Base case: stop recursion when namespace has no further children
  // and return an empty node, which will not be appended since we have a custom version of CTML
  if (ns.records.size() == 0 && ns.enums.size() == 0 && ns.namespaces.size() == 0) {
    return CTML::Node("");
  }

  auto node  = CTML::Node("li.is-family-code#" + ns.ID.str(), ns.name);
  auto subUL = CTML::Node("ul");

  const std::vector<hdoc::types::SymbolID> childNamespaces = getSortedIDs(ns.namespaces, index.namespaces);
  const std::vector<hdoc::types::SymbolID> childRecords    = getSortedIDs(ns.records, index.records);
  const std::vector<hdoc::types::SymbolID> childEnums      = getSortedIDs(ns.enums, index.enums);

  for (const auto& childID : childNamespaces) {
    auto childNode = printNamespace(index.namespaces.entries.at(childID), index);
    subUL.AddChild(childNode);
  }
  for (const auto& childID : childRecords) {
    const hdoc::types::RecordSymbol s = index.records.entries.at(childID);
    subUL.AddChild(
        CTML::Node("li.is-family-code").AddChild(CTML::Node("a", s.type + " " + s.name).SetAttr("href", s.url())));
  }
  for (const auto& childID : childEnums) {
    const hdoc::types::EnumSymbol s = index.enums.entries.at(childID);
    subUL.AddChild(
        CTML::Node("li.is-family-code").AddChild(CTML::Node("a", s.type + " " + s.name).SetAttr("href", s.url())));
  }
  return node.AddChild(subUL);
}

/// Print all of the namespaces in a project in a nice tree-view
void hdoc::serde::HTMLWriter::printNamespaces() const {
  CTML::Node main("main");
  main.AddChild(CTML::Node("h1", "Namespaces"));

  CTML::Node namespaceTree("ul");

  for (const auto& id : getSortedIDs(map2vec(this->index->namespaces), this->index->namespaces)) {
    const auto& ns = this->index->namespaces.entries.at(id);
    // Only recurse root namespaces (that have no parents)
    if (ns.parentNamespaceID.raw() != 0) {
      continue;
    }
    namespaceTree.AddChild(printNamespace(ns, *this->index));
  }
  if (this->index->namespaces.entries.size() == 0) {
    main.AddChild(CTML::Node("p", "No namespaces were declared in this project."));
  } else {
    main.AddChild(namespaceTree);
  }
  printNewPage(*this->cfg,
               main,
               this->cfg->outputDir / "namespaces.html",
               "Namespaces: " + this->cfg->projectName + " " + this->cfg->projectVersion + " documentation");
}

/// Print an enum to main
void hdoc::serde::HTMLWriter::printEnum(const hdoc::types::EnumSymbol& e) const {
  CTML::Node        main("main");
  const std::string pageTitle = e.type + " " + e.name;
  main.AddChild(CTML::Node("h1", pageTitle));

  // Description
  if (e.briefComment != "" || e.docComment != "") {
    main.AddChild(CTML::Node("h2", "Description"));
  }
  if (e.briefComment != "") {
    main.AddChild(CTML::Node("p", e.briefComment));
  }
  if (e.docComment != "") {
    main.AddChild(CTML::Node("p", e.docComment));
  }
  main.AddChild(getDeclaredAtNode(e, this->cfg->gitRepoURL));

  // Enum members in table format
  main.AddChild(CTML::Node("h2", "Enumerators"));
  if (e.members.size() > 0) {
    // Table and table header nodes
    CTML::Node table("table.table is-narrow is-hoverable");
    CTML::Node table_header_row("tr");
    table_header_row.AddChild(CTML::Node("th", "Name"));
    table_header_row.AddChild(CTML::Node("th", "Value"));
    table_header_row.AddChild(CTML::Node("th", "Comment"));
    table.AddChild(table_header_row);

    // Table rows: one row per enum member
    for (const auto& member : e.members) {
      CTML::Node table_row("tr");
      table_row.AddChild(CTML::Node("td.is-family-code", member.name));
      table_row.AddChild(CTML::Node("td.is-family-code", std::to_string(member.value)));
      table_row.AddChild(CTML::Node("td", member.docComment));
      table.AddChild(table_row);
    }
    main.AddChild(table);
  }

  printNewPage(*this->cfg,
               main,
               this->cfg->outputDir / e.url(),
               pageTitle + ": " + this->cfg->projectName + " " + this->cfg->projectVersion + " documentation",
               getBreadcrumbNode(e.type, e, *this->index));
}

/// Print all of the enums in a project
void hdoc::serde::HTMLWriter::printEnums() const {
  CTML::Node main("main");
  main.AddChild(CTML::Node("h1", "Enums"));

  CTML::Node ul("ul");
  for (const auto& id : getSortedIDs(map2vec(this->index->enums), this->index->enums)) {
    const auto& e = this->index->enums.entries.at(id);
    ul.AddChild(CTML::Node("li")
                    .AddChild(CTML::Node("a.is-family-code", e.type + " " + e.name).SetAttr("href", e.url()))
                    .AppendText(getSymbolBlurb(e)));
    this->pool.async([&](const hdoc::types::EnumSymbol& en) { printEnum(en); }, e);
  }
  this->pool.wait();
  main.AddChild(CTML::Node("h2", "Overview"));
  if (this->index->enums.entries.size() == 0) {
    main.AddChild(CTML::Node("p", "No enums were declared in this project."));
  } else {
    main.AddChild(ul);
  }
  printNewPage(*this->cfg,
               main,
               this->cfg->outputDir / "enums.html",
               "Enums: " + this->cfg->projectName + " " + this->cfg->projectVersion + " documentation");
}

void hdoc::serde::HTMLWriter::printSearchPage() const {
  CTML::Node main("main");

  main.AddChild(CTML::Node("h1", "Search"));
  const auto noscriptTagText = R"(Search requires Javascript to be enabled.
No data leaves your machine as part of the search process.
We have left the Javascript code unminified so that you are able to inspect it yourself should you choose to do so.)";
  main.AddChild(CTML::Node("noscript").AddChild(CTML::Node(noscriptTagText)));
  const auto input = CTML::Node("input.input is-primary#search")
                         .SetAttr("type", "search")
                         .SetAttr("autocomplete", "off")
                         .SetAttr("onkeyup", "updateSearchResults()")
                         .SetAttr("style", "display: none");
  main.AddChild(input);
  main.AddChild(CTML::Node("p#info", "Loading index of all symbols. This may take time for large codebases."));
  main.AddChild(CTML::Node("div.list is-hoverable#results").SetAttr("style", "display: none"));
  main.AddChild(
      CTML::Node("script").SetAttr("src", "https://cdn.jsdelivr.net/npm/minisearch@2.4.1/dist/umd/index.min.js"));
  main.AddChild(CTML::Node("script").SetAttr("src", "search.js"));
  printNewPage(*this->cfg,
               main,
               this->cfg->outputDir / "search.html",
               "Search: " + this->cfg->projectName + " " + this->cfg->projectVersion + " documentation");

  std::error_code      ec;
  llvm::raw_fd_ostream jsonPath((cfg->outputDir / "index.json").string(), ec);
  llvm::json::OStream  json(jsonPath);

  json.array([&] {
    for (const auto& s : this->index->functions.entries)
      json.object([&] {
        auto& f = s.second;
        json.attribute("sid", f.isRecordMember ? f.parentNamespaceID.str() + ".html#" + f.ID.str() : f.ID.str());
        json.attribute("name", f.name);
        json.attribute("decl", f.proto);
        json.attribute("type", f.isRecordMember ? 0 : 1);
      });

    for (const auto& s : this->index->records.entries) {
      json.object([&] {
        auto& c = s.second;
        json.attribute("sid", c.ID.str());
        json.attribute("name", c.name);
        json.attribute("decl", c.proto);
        if (c.type == "struct") {
          json.attribute("type", 2);
        } else if (c.type == "class") {
          json.attribute("type", 3);
        } else {
          json.attribute("type", 4);
        }
      });
    }

    for (const auto& s : this->index->enums.entries) {
      json.object([&] {
        auto& e = s.second;
        json.attribute("sid", e.ID.str());
        json.attribute("name", e.name);
        json.attribute("decl", e.name);
        json.attribute("type", 5);
      });

      for (const auto& ev : s.second.members) {
        json.object([&] {
          auto& e = s.second;
          json.attribute("sid", e.ID.str());
          json.attribute("name", ev.name);
          json.attribute("decl", e.name + "::" + ev.name);
          json.attribute("type", 6);
        });
      }
    }
  });
}

/// Print the homepage of the documentation
void hdoc::serde::HTMLWriter::printProjectIndex() const {
  CTML::Node        main("main");
  const std::string pageTitle = this->cfg->projectName + " " + this->cfg->projectVersion + " documentation";

  // If index markdown page was supplied, convert it to markdown and print it
  if (this->cfg->homepage != "") {
    hdoc::utils::MarkdownConverter converter(this->cfg->homepage);
    main = converter.getHTMLNode();
  }
  // Otherwise, create a simple page with links to the documentation
  else {
    main.AddChild(CTML::Node("h1", pageTitle));
    CTML::Node ul("ul");

    ul.AddChild(CTML::Node("li").AddChild(CTML::Node("a", "Records").SetAttr("href", "records.html")));
    ul.AddChild(CTML::Node("li").AddChild(CTML::Node("a", "Functions").SetAttr("href", "functions.html")));
    ul.AddChild(CTML::Node("li").AddChild(CTML::Node("a", "Enums").SetAttr("href", "enums.html")));
    ul.AddChild(CTML::Node("li").AddChild(CTML::Node("a", "Namespaces").SetAttr("href", "namespaces.html")));
    main.AddChild(ul);
  }

  printNewPage(*this->cfg, main, this->cfg->outputDir / "index.html", pageTitle);
}

void hdoc::serde::HTMLWriter::processMarkdownFiles() const {
  for (const auto& f : this->cfg->mdPaths) {
    spdlog::info("Processing markdown file {}", f.string());
    hdoc::utils::MarkdownConverter converter(f);
    CTML::Node                     main      = converter.getHTMLNode();
    std::string                    filename  = "doc" + f.filename().replace_extension("html").string();
    std::string                    pageTitle = f.filename().stem().string();
    printNewPage(*this->cfg, main, this->cfg->outputDir / filename, pageTitle);
  }
}
