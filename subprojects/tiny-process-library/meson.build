project('tiny-process-library', 'cpp', version: 'v2.0.4', default_options: ['warning_level=3'])

src = [
  'process.cpp',
  'process_unix.cpp',
]

inc = include_directories('tpl')
lib = static_library('tiny-process-library', src, include_directories: inc)
dep = declare_dependency(link_with: lib, include_directories: inc)
