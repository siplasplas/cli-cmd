# cli-cmd
# CLI Commander - Modern C++ Command Line Parser

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![Build Status](https://github.com/siplasplas/cli-cmd/actions/workflows/cmake.yml/badge.svg)](https://github.com/siplasplas/cli-cmd/actions)

A C++ CLI argument parsing library inspired by Git/GCC, featuring:
- Hierarchical commands (categories > subcategories > commands)
- Automatic help generation
- Positional argument validation
- Configurable parsing modes (Git/GCC style) (planning)
- 
## Installation
Add as submodule to yout repository.
Library has two modes: header-only and link. 
To use header-only, simply #include "cli-cmd.hpp"; 
to use link mode (if we have multipe .cpp to whicj ust inludes, but now 
library code is so small, header-only is enough),
* #include "cli-cmd.h"
* add two *.src files to project
* or ad these files to static library target and add this library

## Quick start
```c++
#include "cli-cmd.hpp"

void mycommand(cli::Application*, cli::Command* commad)
{
    std::cout << "hello from handler!";
    std::cout << "   mycmd has " << commad->positionalArgs.size() << " positional arguments"
        << std::endl;
}

int main(int argc, char** argv) {
    cli::Application app("first");
    auto cmd = app.addSubcomand(mycommand, "mycmd", "My example command");
    cmd->setPositionalArgsLimits(0, 1);
    app.run(argc, argv);
    return 0;
}
```
Build with
```shell
  g++ -Wall -Wextra -I../../include -std=c++17 -O2 first.cpp -o program
```

## Supported Command-Line Argument Formats
This CLI library supports five distinct formats of command-line arguments, 
modeled after common conventions seen in tools like git, gcc, and UNIX-style CLI programs:

#### Bare ASCII Identifier – Command or Positional Argument
   A string of at least one character, composed only of ASCII letters (a–z, A–Z) and dashes (-).
   Cannot start or end with a dash.

   Example: clone, init-repo, status-check

   Interpretation:<br>
   In multi-command style (like git), this represents a command and must be the first argument after the program name (argv[1]).
   In single-command style (like gcc), it's a positional argument, and may appear in any position.
#### Short Flag or Option – One Dash + One Alphanumeric Character
   Format: exactly two characters, where the first is a single dash (-), and the second is an ASCII letter or digit.
   
   Examples:

        -v (flag: verbose)
        -o (option: output – requires a value in the next argument)

#### Long Flag or Option – Double Dash + Word
   Format:

        Starts with --
        Must be at least 4 characters total
        The character after -- must be an ASCII letter or digit, not a dash
        May contain internal dashes, but cannot end with a dash
   Examples:
   
        --help, --output, --dry-run
#### Compact Flag Grouping
   Format: -abc, where a, b, and c are individual flags
   No dashes allowed inside
   
Examples:
 
        -vg is equivalent to -v -g
        -abc expands to three flags
Flags and options in one group:

If the last character in the group is an option (takes a value), it must be last:

        Value via separate arg: myprog mycmd -vo path
        Or inline with =: myprog mycmd -vo=path
#### Free-form Arguments – Values Passed to Options or as Positional Arguments
   Any string (including special characters like /, :, . or even -) is allowed as a value to an option or as a positional argument, provided it's not interpreted as a flag or command.
   
   Examples:

        myprog clone https://example.com/repo.git
        myprog -o ./build/output.bin
        myprog run --script ./test.sh
   These are interpreted based on context - e.g., after -o, any value is accepted.


## Declaring Optional, Required, and Defaulted Options

This library provides three distinct methods for declaring options (key–value pairs) depending on how strictly they must appear on the command line:
#### addOption(...) – Optional Option

Registers an optional option — one that may be provided by the user, but is not required.

    If the option is missing, the associated value is not set.

    Example:

    cmd.addOption("--output").shorthand('o').expect("path");

    Use case: the output file path may or may not be specified.

#### addReqOption(...) – Required Option

Declares a required option — the program will fail if the user does not provide it.

    Ideal for options like --config or --input that are mandatory.

    Example:

    cmd.addReqOption("--config").expect("path");

#### addDefOption(...) – Option with Default Value

Defines an option that has a default value if it is not present on the command line.

    ⚠️ Note: This does not mean the option is provided without an argument.
    It means the option can be omitted entirely, and its value will default internally.

    Typical for options like --directory where . (current directory) is a reasonable fallback.

    Example:

    cmd.addDefOption("--directory", ".").expect("path");

