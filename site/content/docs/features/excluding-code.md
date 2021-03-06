+++
title = "Excluding Code"
template = "doc-page.html"
weight = 300
description = "hdoc lets you exclude code from its scanner, so tests and other special code doesn't get included in your documentation."
+++

# Excluding code

Many projects have parts of their codebase that don't need to be included in their documentation:
 - Test code, such as unit tests or test harnesses
 - Third-party libraries that are kept alongside the project's code
 - Autogenerated code

Excluding parts of the codebase allows for your documentation to stay clean and focused.

## How to exclude a file or folder

You can exclude a file or folder by adding a path to the `[ignore]` section of your `.hdoc.toml` file.
The example below shows you how to do it.

```toml
# other parts of the file are omitted for this example

[ignore]
# Symbols from any of the following path fragments will be ignored
paths = [
  "/thirdparty/",
  "/src/tests/unittests/",
  "_autogenerated.cpp"
  # more paths can be added as needed
]

# ... more project information
```

hdoc will compare the path of every symbol it finds to the paths you added to the `[ignore]` section.
If any part of the path you added to the `[ignore]` section appears in the symbol's path, the symbol will be discarded.
In essence, a substring match is used to compared ignore paths to symbol paths.

Below is a list of made up paths to code in a repository.
Each path is either processed or ignored, depending on if there is a match to the ignore paths from the example above.
You can see how each path is processed.

```
/home/user/project/src/main.cpp -> ✅ processed
/home/user/project/src/interface_autogenerated.cpp -> ❌ ignored
/home/user/project/src/interface_autogenerated.hpp -> ✅ processed
/home/user/project/src/tests/testfoo.cpp -> ✅ processed
/home/user/project/src/tests/unittests/testbar.cpp -> ❌ ignored
/home/user/project/thirdparty/libabc/libabc.hpp -> ❌ ignored
/home/user/project/third_party/libxyz/libxyz.hpp -> ✅ processed (note the typo!)
```
