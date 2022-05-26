project('cmark-gfm', 'c', version: '0.29.0.gfm.3', default_options: ['warning_level=3'])

src = [
  'src/cmark.c',
  'src/arena.c',
  'src/footnotes.c',
  'src/linked_list.c',
  'src/map.c',
  'src/plaintext.c',
  'src/plugin.c',
  'src/registry.c',
  'src/syntax_extension.c',
  'src/node.c',
  'src/iterator.c',
  'src/blocks.c',
  'src/inlines.c',
  'src/scanners.c',
  'src/utf8.c',
  'src/buffer.c',
  'src/references.c',
  'src/render.c',
  'src/man.c',
  'src/xml.c',
  'src/html.c',
  'src/commonmark.c',
  'src/latex.c',
  'src/houdini_href_e.c',
  'src/houdini_html_e.c',
  'src/houdini_html_u.c',
  'src/cmark_ctype.c',
  'extensions/autolink.c',
  'extensions/autolink.h',
  'extensions/cmark-gfm-core-extensions.h',
  'extensions/core-extensions.c',
  'extensions/ext_scanners.c',
  'extensions/ext_scanners.h',
  'extensions/strikethrough.c',
  'extensions/strikethrough.h',
  'extensions/table.c',
  'extensions/table.h',
  'extensions/tagfilter.c',
  'extensions/tagfilter.h',
  'extensions/tasklist.c',
  'extensions/tasklist.h',
]


add_project_arguments('-fvisibility=hidden', language: 'c')
# The GitHub fork has lots of unused parameters warnings, which I don't want to see.
add_project_arguments('-Wno-unused-parameter', language: 'c')
inc = []
inc += include_directories('src')
inc += include_directories('extensions')
lib = static_library('cmark-gfm', src, include_directories: inc)
dep = declare_dependency(link_with: lib, include_directories: inc)
