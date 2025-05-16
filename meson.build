project('cli-cmd', 'cpp',
        version : '0.9.0',
        default_options : [
          'warning_level=3',
          #'werror=true',
          'cpp_std=c++17'
        ])

clicmd_mode = get_option('clicmd_mode')

inc = include_directories('include')

if clicmd_mode == 0
  message('cli-cmd: header-only mode selected')
  add_project_arguments('-DCLICMD_HEADER_ONLY', language : 'cpp')
  lib_dep = declare_dependency(include_directories : inc)

elif clicmd_mode == 1
  message('cli-cmd: source inclusion mode selected')
  clicmd_sources = files(
    'src/cli-cmd.cpp',
    'src/distance.cpp',
    'src/util.cpp',
    'src/validator.cpp',
  )
  lib_dep = declare_dependency(sources : clicmd_sources, include_directories : inc)

elif clicmd_mode == 2
  message('cli-cmd: static library mode selected')
  clicmd_sources = files(
    'src/cli-cmd.cpp',
    'src/distance.cpp',
    'src/util.cpp',
  )
  clicmd_lib = static_library(
    'cli-cmd',
    sources: clicmd_sources,
    include_directories: inc,
  )
  lib_dep = declare_dependency(link_with : clicmd_lib, include_directories : inc)
endif

first_sources = [
  'examples/first/first.cpp',
]

executable('first',
           first_sources,
           dependencies : lib_dep,
)

mycli_sources = [
  'examples/mycli/mycli.cpp',
]

executable('mycli',
           mycli_sources,
           dependencies : lib_dep,
)


likegit_sources = [
  'examples/likegit/main.cpp',
  'examples/likegit/developerfacing.cpp',
  'examples/likegit/external.cpp',
  'examples/likegit/interacting.cpp',
  'examples/likegit/interrogators.cpp',
  'examples/likegit/lowlevelinternal.cpp',
  'examples/likegit/lowlevelinterrogators.cpp',
  'examples/likegit/lowlevelmanipulators.cpp',
  'examples/likegit/lowlevelsyncing.cpp',
  'examples/likegit/manipulators.cpp',
  'examples/likegit/porcelain.cpp',
  'examples/likegit/userfacing.cpp'
]

executable('likegit',
  likegit_sources,
  dependencies : lib_dep,
)

likegcc_sources = [
  'examples/likegcc/main.cpp',
  'examples/likegcc/options.cpp',
]

executable('likegcc',
  likegcc_sources,
  dependencies : lib_dep,
)

gtest_dep = dependency('gtest', required: true)
gmock_dep = dependency('gmock', required: true)
gtest_main = dependency('gtest_main', required: true)

test_sources = files(
  'tests/test_arguments.cpp',
  'tests/test_similar.cpp',
  'tests/test_flag.cpp',
  'tests/test_token_classification.cpp',
  'tests/test_parameters.cpp',
  'tests/test_expect.cpp',
  'tests/test_global_local.cpp'
)

test_exe = executable(
  'cli_tests',
  test_sources,
  include_directories: inc,
  dependencies: [gtest_dep, gtest_main, gmock_dep, lib_dep],
)

test('cli-tests', test_exe)