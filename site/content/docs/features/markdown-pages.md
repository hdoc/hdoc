+++
title = "Markdown Pages"
template = "doc-page.html"
weight = 100
description = "hdoc's integrated Markdown pages let you use hdoc as your single source of documentation."
+++

# Integrated Markdown Pages

Instead of having to spread your documentation across one site for the API reference, another site for build instructions, and a wiki for usage and architectural docs, hdoc lets you put everything in once place.

The integrated markdown pages feature lets you write Markdown files which will be converted to webpages and placed alongside the API reference the hdoc automatically generates.
hdoc's parser supports the [CommonMark specification](https://spec.commonmark.org/).

## Adding pages

Adding pages is simple.
First, make a Markdown document.
Typical content includes build and usage instructions, contribution guidelines, and manuals.
You can find a description of Markdown syntax and usage [here](https://daringfireball.net/projects/markdown/syntax).

If you have existing written documentation in a format other than Markdown, the [pandoc](https://pandoc.org/) document converter can help you convert it to Markdown.

Next, add a new section in your `.hdoc.toml` file.

```toml
# other parts of the file are omitted for this example

[pages]
# These Markdown files will be converted to webpages and listed in the sidebar
paths = [
  "docs/GettingStarted.md",
  "docs/Building.md",
  "docs/CodingStandards.md",
  "docs/ReleaseNotes.md",
  # more Markdown files can be added as needed
]

# ... more project information
```

These Markdown files will be converted to webpages the next time you run hdoc.
You can find them under the special "Pages" section of the sidebar, where the name of the file you specified in `.hdoc.toml` is a link to the webpage.

## Documentation homepage

hdoc will create a basic homepage for your documentation by default.
However, hdoc can optionally make the homepage of your documentation a Markdown page that you specify, in place of the automatically generated one.
This is done by adding the `homepage` option to the `pages` section of your `.hdoc.toml` file:

```toml
[pages]
homepage = "docs/homepage.md" # Only one file can be the homepage
paths = [
  "docs/GettingStarted.md",
  "docs/Building.md",
  "docs/CodingStandards.md",
  "docs/ReleaseNotes.md",
]
```

When you open your documentation in the browser you should now see the Markdown file as a webpage when you click the "Homepage" link at the top of the sidebar.

## Limitations due to security

hdoc limits what you can put in your Markdown content to avoid cross-site scripting (XSS) attacks.
This means that raw HTML and links to malicious content are ignored when converting Markdown documents to a webpage.
