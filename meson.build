project('cli-cmd', 'cpp',
        version : '0.0.1',
        default_options : ['warning_level=3',
                           'werror=true',
                           'cpp_std=c++14'])

cli_cmd = executable('cli_cmd', 'examples/main.cpp', install : true)

test('test', cli_cmd)