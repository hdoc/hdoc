project('hdoc', 'cpp', version: '1.0.1', default_options: ['cpp_std=c++17', 'warning_level=3'])

if get_option('static_hdoc') == true
  dep_llvm = dependency('LLVM', method: 'config-tool', static: true, include_type: 'system')
  clang_deps_ldflags = [
    '-lclangAST',
    '-lclangASTMatchers',
    '-lclangAnalysis',
    '-lclangBasic',
    '-lclangDriver',
    '-lclangEdit',
    '-lclangFormat',
    '-lclangFrontend',
    '-lclangIndex',
    '-lclangLex',
    '-lclangParse',
    '-lclangRewrite',
    '-lclangSema',
    '-lclangSerialization',
    '-lclangTooling',
    '-lclangToolingCore',
    '-lclangToolingInclusions',
  ]
  dep_clang = declare_dependency(link_args: clang_deps_ldflags)
else
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
endif

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
deps += subproject('cmark').get_variable('dep')
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
  assets_src,
]
lib = static_library('hdoc', sources: src, include_directories: inc, dependencies: deps)
libdeps = declare_dependency(dependencies: deps, include_directories: inc, link_with: lib)

executable('hdoc', sources: 'src/main.cpp', dependencies: libdeps)
executable('hdoc-client', sources: 'src/client-main.cpp', dependencies: libdeps)

test_src = [
  'index-tests/test.cpp',
  'index-tests/common.cpp',
  'index-tests/test-records.cpp',
  'index-tests/test-unions.cpp',
  'index-tests/test-functions.cpp',
  'index-tests/test-enums.cpp',
  'index-tests/test-namespaces.cpp',
  'index-tests/test-typedefs.cpp',
  'index-tests/test-inheritance.cpp',
  'index-tests/test-constructors.cpp',
  'index-tests/test-operators.cpp',
  'index-tests/test-templates.cpp',
  'index-tests/test-comments-records.cpp',
  'index-tests/test-comments-functions.cpp',
  'index-tests/test-comments-enums.cpp',
  'index-tests/test-comments-namespaces.cpp',
]
executable('index-tests', sources: test_src, dependencies: libdeps, cpp_args: '-DHDOC_TEST_CODE')
