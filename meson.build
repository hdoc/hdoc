project('hdoc', 'c', 'cpp', version: '1.4.1', default_options: ['cpp_std=c++20', 'warning_level=3'])

dep_llvm = dependency('LLVM', include_type: 'system')
llvm_libdir = dep_llvm.get_variable(cmake: 'LLVM_LIBRARY_DIR', configtool: 'libdir')
dep_clang = meson.get_compiler('cpp').find_library('clang-cpp', dirs: llvm_libdir, required: false)
if not dep_clang.found()
  clang_modules = [
    'clangTooling',
    'clangToolingInclusions',
    'clangToolingCore',
    'clangFrontend',
    'clangAST',
    'clangIndex',
    'clangBasic',
  ]
  dep_clang = dependency('Clang', include_type: 'system', method: 'cmake', modules: clang_modules)
endif

# Disable RTTI
add_project_arguments('-fno-rtti', language: 'cpp')

# Generate char arrays from asset files using xxd -i
asset_files = [
  'site/content/oss.md',
  'assets/apple-touch-icon.png',
  'assets/favicon-16x16.png',
  'assets/favicon-32x32.png',
  'assets/favicon.ico',
  'assets/styles.css',
  'assets/search.js',
  'assets/worker.js',
  'assets/highlight.min.js',
  'assets/katex.min.js',
  'assets/katex.min.css',
  'assets/auto-render.min.js',
  'assets/index.min.js',
  'schemas/hdoc-payload-schema.json',
]
gen = generator(find_program('xxd'),
                output: '@PLAINNAME@.cpp',
                arguments: ['-i', '@INPUT@', '@OUTPUT@'])
assets_src = gen.process(asset_files)

# Subproject configuration #defines
add_project_arguments('-DCPPHTTPLIB_OPENSSL_SUPPORT', language: 'cpp')
add_project_arguments('-DCPPHTTPLIB_ZLIB_SUPPORT', language: 'cpp')
add_project_arguments('-DRAPIDJSON_HAS_STDSTRING', language: 'cpp')

deps = []
deps += dep_llvm
deps += dep_clang
deps += dependency('threads')
deps += dependency('openssl')
deps += subproject('zlib', default_options: ['default_library=static']).get_variable('zlib_dep')
deps += subproject('rapidjson').get_variable('rapidjson_dep')
deps += subproject('cmark-gfm', default_options: ['default_library=static']).get_variable('cmark_gfm_dep')
deps += subproject('spdlog').get_variable('spdlog_dep')
deps += subproject('ctml').get_variable('ctml_dep')
deps += subproject('argparse').get_variable('argparse_dep')
deps += subproject('tomlplusplus').get_variable('tomlplusplus_dep')
deps += subproject('doctest').get_variable('doctest_dep')
deps += subproject('cpp-httplib', default_options: ['cpp-httplib_brotli=disabled']).get_variable('cpp_httplib_dep')

inc = include_directories('src')
src = [
  'src/frontend/Frontend.cpp',
  'src/indexer/Indexer.cpp',
  'src/indexer/Matchers.cpp',
  'src/indexer/MatcherUtils.cpp',
  'src/serde/SerdeUtils.cpp',
  'src/serde/JSONDeserializer.cpp',
  'src/serde/HTMLWriter.cpp',
  'src/serde/Serialization.cpp',
  'src/support/ParallelExecutor.cpp',
  'src/support/StringUtils.cpp',
  'src/support/MarkdownConverter.cpp',
  assets_src,
]
lib = static_library('hdoc', sources: src, include_directories: inc, dependencies: deps)
libdeps = declare_dependency(dependencies: deps, include_directories: inc, link_with: lib)

executable('hdoc', sources: 'src/main.cpp', dependencies: libdeps, install: true)
executable('hdoc-online', sources: 'src/hdoc-online-main.cpp', dependencies: libdeps, install: true)

tests_src = [
  'tests/TestUtils.cpp',
  'tests/hdoc-tests-main.cpp',
  'tests/index-tests/test-records.cpp',
  'tests/index-tests/test-unions.cpp',
  'tests/index-tests/test-functions.cpp',
  'tests/index-tests/test-enums.cpp',
  'tests/index-tests/test-namespaces.cpp',
  'tests/index-tests/test-typedefs.cpp',
  'tests/index-tests/test-inheritance.cpp',
  'tests/index-tests/test-constructors.cpp',
  'tests/index-tests/test-operators.cpp',
  'tests/index-tests/test-templates.cpp',
  'tests/index-tests/test-comments-records.cpp',
  'tests/index-tests/test-comments-functions.cpp',
  'tests/index-tests/test-comments-enums.cpp',
  'tests/index-tests/test-comments-namespaces.cpp',
  'tests/index-tests/test-comments-templates.cpp',
  'tests/json-tests/json-tests-records.cpp',
  'tests/json-tests/json-tests-functions.cpp',
  'tests/json-tests/json-tests-enums.cpp',
  'tests/json-tests/json-tests-namespaces.cpp',
  'tests/json-tests/json-tests-schema-validation.cpp',
  'tests/unit-tests/test.cpp',
]
executable('hdoc-tests', sources: tests_src, dependencies: libdeps)
