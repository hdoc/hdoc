+++
title = "Troubleshooting"
template = "doc-page.html"
weight = 3
description = "Having trouble? Check out Troubleshooting for help with some common issues."
+++

# Troubleshooting

In the unlikely event that you're having problems with hdoc, this page may help.
Information is added this page as we receive reports from users.

## Missing include paths
While analyzing your code, hdoc may not be able to find some header files.
This typically manifests itself in an error in the terminal similar to the one below:

```bash
main.cpp:1:10: fatal error: cannot open file 'iostream': No such file or directory
#include <iostream>
         ^
1 error generated.
```

hdoc does its best to automatically locate include paths by looking up the include search paths of the system compiler.
However, sometimes you may need to help it by manually telling it to search additional paths.

This is done by adding paths in the `[includes]` section of your `.hdoc.toml` file.
The example below shows how to do it.

```toml
# other parts of the file are omitted for this example

[includes]
# Add the following include paths so hdoc can find my special headers
paths = [
  "/usr/local/special-path",
  "/usr/local/opt/libsomething/include",
  # more paths can be added as needed
]

# ... more project information
```

### Manually defining include paths

You can also disable hdoc's automatic detection of include paths by disabling the `use_system_includes` option in the `.hdoc.toml` file.
Here's how to do that:

```toml
[includes]
use_system_includes = false
```

Once system includes are disabled, you can your own include paths to the `paths` option as shown above.
