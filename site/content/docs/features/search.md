+++
title = "Search"
template = "doc-page.html"
weight = 200
description = "hdoc's instant search capability helps you find what you need quickly and easily."
+++

# Search

Special attention has been paid to making searching your codebase a pleasant experience.
The built-in search tool allows you to search through all the symbols defined in your codebase, assuming they haven't been [intentionally excluded](@/docs/features/excluding-code.md).

hdoc makes the following items searchable:
  - Functions
  - Methods
  - Enums
  - Enum values
  - Classes
  - Structs
  - Unions

There is no configuration needed to enable the search feature.
The search interface can be accessed by going to the "Search" link in the sidebar of your documentation site.
You may have to wait for the search index to load before using it, especially if you have a large project.

Once the search index has loaded, searching is an instant experience and new items appear after every keystroke.
Symbols can be searched by their names or declarations, so if you know the name of a symbol you can type its name in and go to its API reference quickly.

The search function also accomodates fuzzy and partial searching.
This means that even if you don't remember the full name of a symbol, you can type part of its name and relevant results will be returned.
