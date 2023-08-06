+++
title = "Configuration File "
slug = "config-file-reference"
template = "doc-page.html"
description = "Reference for .hdoc.toml, hdoc's configuration file format."
+++


# Configuration file reference

hdoc is configured using the `.hdoc.toml` configuration file.
The configuration file uses the [TOML format](https://github.com/toml-lang/toml/blob/master/README.md).
This page shows all of the options available in the `.hdoc.toml` configuration file, broken down by section.
Each option has a short description as well as an example of its usage.

## `project`

The project section contains the vital information needed for hdoc to recognize your project.
This is a required section.

### `name`

The project name is the short name for your project which is used on the homepage and sidebar to identify your project on the documentation site.
It is a string.
It is required.

```toml
[project]
name = "myproject"
```

### `version`

The project version is the version of your project which is used on the homepage and sidebar to identify your project on the documentation site.
It is a string.
It is optional and defaults to an empty string.

```toml
[project]
version = "1.0.0"
```

### `num_threads`

The number of threads to be used during indexing of the project.
Increasing the number of threads can make hdoc finish its job faster on multicore machines.
A value of 0 indicates that all available system threads will be used (i.e. a machine with 8 logical cores will use 8 threads).
It is an integer, which must be greater than or equal to 0.
It is optional and defaults to 0.

```toml
[project]
num_threads = 8
```

### `git_repo_url`

URL to a GitHub or GitLab repository where source code for the current project is stored.
This is used to link functions, records, and enums to their original declarations in the source code.
The URL must end with a trailing slash.
The value is a string, and is optional.
If the value is not supplied, declarations will not be linked back to the source code.

```toml
[project]
git_repo_url = "https://github.com/hdoc/hdoc/"
```

### `git_default_branch`

The name of the branch to use when linking to definitions on GitHub or GitLab.
A commit hash or a tag can be used instead to pin the documentation to a given version of the code, while using a branch designation (such as "main") will use the latest commit on that branch for all links.
The value is a string, and is optional.
If the value is not supplied, declarations will not be linked back to the source code.

```toml
git_default_branch = "master"

# Alternatively, pin all links to the 3d9b3c0... commit
# git_default_branch = "3d9b3c0f6d21b7dc79318da394afcdd2d3e077cc"

# Or pin all links to the 1.3.2 tag
# git_default_branch = "1.3.2"
```

## `paths`

The paths section contains information needed by hdoc to parse your codebase and to know where to put its files.
This is a required section.

### `compile_commands`

The location of a `compile_commands.json` file is required for hdoc to be able to parse your codebase.
It is a string that represents a path to a `compile_commands.json` file on your filesystem.
The path can be absolute, or relative to the location of the `.hdoc.toml` file.
It is required.

```toml
[paths]
compile_commands = "build/compile_commands.json"
```

### `output_dir`

The output directory is the directory in which hdoc will output its static HTML documentation.
The directory does not need to exist prior to running hdoc.
If it does not exist, hdoc will create it automatically.
It is a string that represents a path on your filesystem.
The path can be absolute, or relative to the location of the `.hdoc.toml` file.
It is required for full versions of hdoc, but optional for "client" versions of hdoc.

```toml
[paths]
output_dir = "docs/hdoc-output"
```

### `input_dir`

hdoc discards source files that are located outside of input directory or its subdirectories.
Specifying `input_dir` allows for having the configuration file separate from your sources, if desired.
If no `input_dir` is specified, it defaults to the CWD.

```toml
[paths]
input_dir = "/path/to/your/source/root"
```

## `includes`

The includes section allows for finer-grained control of how hdoc finds included files.
This is an optional section.

### `use_system_includes`
By default hdoc tries uses the system compiler's header search paths to find files that are included by the preprocessor.
This feature can be disabled with the `use_system_includes` option.
This is a boolean value that is true by default and can be overridden.
It is optional.

```toml
[includes]
use_system_includes = false
```

### `paths`

The paths variable lets you list an array of paths to directories that hdoc will use when searching for headers.
It is optional.

```toml
[includes]
paths = [
    "/usr/include",
    "/usr/local/opt/llvm/include",
    # Other paths as needed
]
```

## `ignore`

The ignore section tells hdoc which parts of the codebase it should ignore.
You can learn more about the ignore process at the [Excluding Code](@/docs/features/excluding-code.md) page.
This is an optional section.

### `paths`

The paths variable lets you control which parts of your codebase will be ignored.
If a symbol is defined in a file whose fully-qualified path is a superset of a string in this option, it will be ignored by hdoc and not included.
This option is an array of strings.
It is optional.

```toml
[ignore]
paths = [
    "/tests/",
    "/src/impl/",
    # Other substrings as needed
]
```

### `ignore_private_members`

Private member variables and private member functions of records are included in the documentation output by default.
Some users prefer to exclude these from the generated documentation since these members are inaccessible to other developers because they are private.
If `ignore_private_members` is set to true, then all private member variables and private member functions of records will be ignored and not present in the HTML output or the search index.
This is a boolean value that is false by default and can be overridden.
It is optional.

```toml
[ignore]
ignore_private_members = true
```

## `pages`

The pages section controls the inclusion of Markdown pages into the generated documentation.
You can learn more about including Markdown pages in the [Markdown Pages](@/docs/features/markdown-pages.md) page.
This is an optional section.

### `homepage`

hdoc will create a basic homepage for your documentation by default.
This can be overridden by specifying a path to a Markdown file that will replace the autogenerated homepage.
The path can be absolute, or relative to the location of the `.hdoc.toml` file.
It is optional.

```toml
[pages]
homepage = "docs/index.md"
```

### `paths`

The paths variable is a list of paths to Markdown files.
These Markdown files will be converted to webpage and placed alongside your API documentation.
The name of each file (without the extension or the directory it is in) is used as the name of the link to that webpage.
This option is an array of strings that represent paths to Markdown files.
It is optional.

```toml
[pages]
paths = [
      "docs/GettingStarted.md",
      "docs/Building.md",
      "docs/CodingStandards.md",
      "docs/ReleaseNotes.md",
      # Other paths as needed
]
```

## `debug`

The debug section contains configuration options meant to be used bringup and debugging of hdoc.
This is an optional section.

### `limit_num_indexed_files`

A user may want to limit the number of files they index if they have a huge codebase and don't want to wait for hdoc to index the entire codebase.
This option allows them to only index a limited number of files for more rapid development.
It is not intended for use in production, only in bring-up.
It is an integer, which must be greater than or equal to 0.
It is optional and defaults to 0.

```toml
[debug]
limit_num_indexed_files = 10
```

### `dump_json_payload`

hdoc can optionally dump its internal data structures in JSON format to the current working directory when this option is set to true.
The resulting file will have the name "hdoc-payload.json".
This option is a boolean value that is false by default and can be overridden.
It is optional.

```toml
[debug]
dump_json_payload = true
```
