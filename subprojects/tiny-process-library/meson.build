project('tiny-process-library', 'cpp', version: 'v2.0.4', default_options: ['warning_level=3'])

src = [
  'process.cpp'
]

if host_machine.system() == 'windows'
  src += 'process_win.cpp'
  add_project_arguments('-D_WIN32', language : 'cpp')
else
  src += 'process_unix.cpp'
endif

inc = include_directories('tpl')
lib = static_library('tiny-process-library', src, include_directories: inc)
dep = declare_dependency(link_with: lib, include_directories: inc)
