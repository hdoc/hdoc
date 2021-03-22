project('cmark', 'c', default_options: ['warning_level=3'])

src = [
  'src/cmark.c',
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
]

add_project_arguments('-fvisibility=hidden', language: 'c')
inc = include_directories('src')
lib = static_library('cmark', src, include_directories: inc)
dep = declare_dependency(link_with: lib, include_directories: inc)
