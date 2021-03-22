project('spdlog', 'cpp', default_options: ['warning_level=3'])

add_project_arguments('-DSPDLOG_COMPILED_LIB', language: 'cpp')

src = [
  'src/async.cpp',
  'src/color_sinks.cpp',
  'src/file_sinks.cpp',
  'src/spdlog.cpp',
  'src/stdout_sinks.cpp',
  'src/cfg.cpp',
  'src/fmt.cpp',
]

inc = include_directories('include')
lib = static_library('spdlog', src, include_directories: inc)
dep = declare_dependency(link_with: lib, include_directories: inc)
