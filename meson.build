project('hdoc', 'cpp', version: '1.2.1', default_options: ['cpp_std=c++17', 'warning_level=3'])

dep_llvm = dependency('LLVM', include_type: 'system')
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

# Disable RTTI
add_project_arguments('-fno-rtti', language: 'cpp')

# Add minimum macOS sdk version needed to use std::filesystem on macOS
if build_machine.system() == 'darwin'
  add_project_arguments('-mmacosx-version-min=10.15', language : 'cpp')
endif

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
]
gen = generator(find_program('xxd'),
                output: '@BASENAME@.cpp',
                arguments: ['-i', '@INPUT@', '@OUTPUT@'])
assets_src = gen.process(asset_files)

# Enable SSL support
add_project_arguments('-DCPPHTTPLIB_OPENSSL_SUPPORT', language: 'cpp')

deps = []
deps += dep_llvm
deps += dep_clang
deps += dependency('threads')
deps += dependency('openssl')
deps += subproject('cmark-gfm').get_variable('dep')
deps += subproject('spdlog').get_variable('dep')
deps += subproject('tiny-process-library').get_variable('dep')
deps += subproject('ctml').get_variable('dep')
deps += subproject('argparse').get_variable('dep')
deps += subproject('toml++').get_variable('dep')
deps += subproject('doctest').get_variable('dep')
deps += subproject('cereal').get_variable('dep')
deps += subproject('cpp-httplib').get_variable('dep')

inc = include_directories('src')
src = [
  'src/frontend/Frontend.cpp',
  'src/indexer/Indexer.cpp',
  'src/indexer/Matchers.cpp',
  'src/indexer/MatcherUtils.cpp',
  'src/serde/HTMLWriter.cpp',
  'src/serde/Serialization.cpp',
  'src/support/ParallelExecutor.cpp',
  'src/support/StringUtils.cpp',
  'src/support/MarkdownConverter.cpp',
  assets_src,
]
lib = static_library('hdoc', sources: src, include_directories: inc, dependencies: deps)
libdeps = declare_dependency(dependencies: deps, include_directories: inc, link_with: lib)

executable('hdoc', sources: 'src/main.cpp', dependencies: libdeps)
executable('hdoc-client', sources: 'src/client-main.cpp', dependencies: libdeps)

unit_tests_src = [
  'tests/unit-tests/test.cpp',
]
executable('unit-tests', sources: unit_tests_src, dependencies: libdeps, cpp_args: '-DHDOC_TEST_CODE')

test_src = [
  'tests/index-tests/test.cpp',
  'tests/index-tests/common.cpp',
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
]
executable('index-tests', sources: test_src, dependencies: libdeps, cpp_args: '-DHDOC_TEST_CODE')
