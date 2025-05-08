project('cli-cmd', 'cpp',
        version : '0.4.0-dev',
        default_options : [
          'warning_level=3',
          #'werror=true',
          'cpp_std=c++17'
        ])

# --- get option from meson_options.txt ---
header_only = get_option('CLICMD_HEADER_ONLY')

# --- definition for header-only ---
if header_only
  add_project_arguments('-DCLICMD_HEADER_ONLY', language : 'cpp')
endif

lib_sources = files([
  'src/cli-cmd.cpp',
  'src/distance.cpp',
])

inc = include_directories('include')
if not header_only
  cli_cmd_lib = static_library('cli-cmd',
    lib_sources,
    include_directories : inc
  )
  lib_dep = declare_dependency(
    link_with : cli_cmd_lib,
    include_directories : inc
  )
else
  lib_dep = declare_dependency(
    include_directories : inc
  )
endif

first_sources = [
  'examples/first/first.cpp',
]

executable('first',
           first_sources,
           dependencies : lib_dep,
           install : true
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
  install : true
)

likegcc_sources = [
  'examples/likegcc/main.cpp',
  'examples/likegcc/options.cpp',
]

executable('likegcc',
  likegcc_sources,
  dependencies : lib_dep,
  install : true
)
