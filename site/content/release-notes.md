+++
title = "Release Notes"
template = "content-page.html"
weight = 1
date = 2020-01-01
description = "What's changed between each release of hdoc, including new features, fixes, and improvements."
+++

# Version 1.1.0 (23 June 2021)

Check out [the accompanying blog post](@/blog/improvements-in-hdoc-1-1-0.md) for more details on the features in hdoc 1.1.0 and videos showing them off.

## New Features
* hdoc now supports HTML links to other symbols defined in a project within the generated documentation.
  - Record member variables and function parameters that are of a known type will automatically have a clickable HTML link taking you directly to their documentation.
  - Many `std` types will also automatically hotlink to their documentation on [cppreference.com](https://en.cppreference.com/w/).
* hdoc now allows you to jump directly to the source code definition of a symbol in GitHub or GitLab from its HTML documentation.
  - hdoc will automatically insert links to the exact file and line where a symbol is stored when you provide a URL to the GitHub or GitLab repository where source code is stored.
  - The URL is specified in the `git_repo_url` option in `.hdoc.toml` See the [configuration file reference](@/docs/reference/config-file-reference.md#git-repo-url) for more information.

## Fixes
* The "Description" heading is now omitted if there are no relevant comments attached to the symbol, whereas previously the "Description" header was printed with nothing below it.
* The message displayed on the Search page to users who do not enable Javascript has been improved and clarified.
* A message is now displayed when no results are found during searches, improving on the empty space that was previously shown.
* The timestamp at the bottom of each generated HTML page is now in UTC instead of EDT.

## Internal Changes
* Many of hdoc's dependencies were updated:
  - LLVM/Clang 12
  - spdlog 1.8.5
  - doctest 2.4.6
  - tiny-process-library v2.0.4
  - toml++ 2.3.0
* A license header indicating hdoc's license and copyright information was added to the top of every source file.
* A single thread pool is now used for all parallel work instead of spinning up and destroying a thread pool for each operation in turn.
* `ParallelExecutor` has been put into the `hdoc::indexer` namespace (previously it was a top-level declaration).
* Improvements to comments and in-source documentation across the project.

# Version 1.0.1 (22 March 2021)

## New Features
* hdoc is now open source and provides free hosting! See the [blog post](@/blog/open-sourcing-hdoc.md) for details.

## Fixes
* hdoc's documentation websites now indicates there were no {Enums,Namespaces,Records} in a project instead of just printing a blank page.
* "Defined at" text was replaced with "Declared at" on HTML pages.

# Version 1.0.0 (4 February 2021)

## New Features
* User documentation is now hosted at docs.hdoc.io.
  - Users must supply the `HDOC_PROJECT_API_KEY` environment variable when running hdoc. The API key is generated when a new project is created in the hdoc console.
  - Documentation is hosted behind a CDN for maximum performance.
  - Enterprise Edition retains the option to output documentation to HTML files on the local filesystem.
* hdoc now uses multiple threads to increase the speed at which it analyzes projects.
  - The `num_threads` option in `.hdoc.toml` controls the number of threads to be used. See the [configuration file reference](@/docs/reference/config-file-reference.md#num-threads) for more information.
  - By default all available threads will be used, corresponding to `num_threads = 0`.
  - Processing speed increase is roughly linear with number of threads, which significantly improves performance on large projects.
* Attribution for open source projects.
  - hdoc relies on several open source projects, and now shows attribution for these projects.
  - Passing the `--oss` flag on the command line will print attribution information.
  - This information is also available on the [hdoc.io website](@/oss.md).
  - To the maintainers and contributors to these projects: thank you!
* [Enterprise Edition] Full support for browsing local documentation using `file:///` URLS.
  - Local HTML output is now fully static and does not require a local HTTP server for most browsing.
  - However, using search functionality still requires a HTTP server.

## Fixes
* Minor whitespace fixes.
  - Several minor fixes to extraneous whitespace that was inserted into the HTML documentation in corner cases.
* It is more accurate to refer to Structs, Classes, and Unions as "Records" instead of using the catch all term "Classes".
  - All references to "Classes" except those that are actually `class` declarations have been renamed to "Records".
* Explicit handling of scoped enums.
  - Scoped enums, or `enum class`, were listed as C-style `enums`. This has been fixed and they are now referred to by the correct name: `enum class`.

# Alpha (17 July 2020)

Initial release.
