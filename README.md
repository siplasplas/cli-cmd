# cli-cmd
# CLI Commander - Modern C++ Command Line Parser

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![Build Status](https://github.com/siplasplas/cli-cmd/actions/workflows/cmake.yml/badge.svg)](https://github.com/siplasplas/cli-cmd/actions)

A C++ CLI argument parsing library inspired by Git/GCC, featuring:
- Commands in categories for help
- Possibility of main category for all commands, and second category for common commands
- Automatic help generation
- Positional argument validation
- Configurable parsing modes (Git/GCC style)

## Prerequisites
```shell
   sudo apt install libgtest-dev libgmock-dev
```
## Installation
Add as submodule to yout repository.
Library has three modes: header-only, link a few *.cpp files or build library linked to program 
To use header-only, simply #include "cli-cmd.hpp"; 
to use link mode (useful if we have multiple .cpp, but now library code is so small, header-only is enough),
* #include "cli-cmd.h"
* add *.src files to project
* or ad these files to static library target and add this library

## Quick start
```c++
#include "cli-cmd.hpp"

void clone_(const cli::Actual* actual)
{
    std::cout << "hello from handler!";
    std::cout << "   mycmd has " << actual->arguments.size() << " positional arguments"
        << std::endl;
}

int main(int argc, char** argv) {
    cli::Application app("first");
    app.addCommand("clone").desc("Clone a repository into a new directory")
            .addArg("repository", "url").addArgs("directory", "path", 0, 1)
            .handler(clone_);
    app.run(argc, argv);
    return 0;
}
```
Build with
```shell
  g++ -Wall -Wextra -I../../include -std=c++17 -O2 first.cpp -o first
```
## Options
### Terminology: Arguments, Options, Flags, and Parameters

This library uses precise terminology to clearly distinguish between different types of command-line inputs. These terms are used consistently throughout the API and documentation:
```
Term Meaning Example
Argument   A positional input that does not start with - or --. Can be required or variadic.            mycli clone https://url
Option     A generic term for anything starting with - or --. This includes both flags and parameters.  --verbose, --output=file
Flag       A boolean switch that only signals presence (true if given, false if omitted).               --verbose, -v
Parameter  An option that requires a value (e.g. a string, path, number).                               --output result.txt, -o x
```
#### Clarifications
* All flags and parameters are referred to as options at the syntax level, but they differ in semantics.
* An argument is always positional (e.g., a repository URL or a file path).
* A parameter is similar to an argument, but it is attached to an option and always has a name (e.g., --output).
* This library treats flags and parameters differently in parsing, validation, and error reporting.

Example
```
mycli build src --release --output bin/app

Token       Category    Notes
mycli       Program     Name of owur cli program = argv[0]
build       Command     Can be rather named subcommand while whole program is command   
src         Argument    Positional input
--release   Flag        Boolean switch
--output    Parameter   Requires a value: bin/app
```
This clear separation allows the library to provide strong validation, smart error messages, 
and advanced features like test JSON output for parsed commandline.

### Supported Command-Line Argument Formats
This CLI library supports eight distinct formats of command-line arguments, 
modeled after common conventions seen in tools like git, gcc, and UNIX-style CLI programs:

#### Bare ASCII Identifier – Command or Positional Argument
   A string of at least one character, composed only of ASCII letters (a–z, A–Z) and dashes (-).
   Cannot start or end with a dash.

   Example: clone, init-repo, status-check

   Interpretation:<br>
   In multi-command style (like git), this represents a command and must be the first argument after the program name ((sub)command = argv[1]).
   In single-command style (like gcc), it's a positional argument, and may appear in any position.
#### Short Flag or Parameter – One Dash + One Alphanumeric Character
   Format: exactly two characters, where the first is a single dash (-), and the second is an ASCII letter or digit.
   
   Examples:

        -v (flag: verbose)
        -o (parameter: output – requires a value in the next argument)

#### Long Flag or Parameter – Double Dash + Word
   Format:

        Starts with --
        Must be at least 4 characters total
        The character after -- must be an ASCII letter or digit, not a dash
        May contain internal dashes, but cannot end with a dash and cannot start with three dash
   Examples:
   
        --help, --output, --dry-run
#### Compact Flag Grouping
   Format: -abc, where a, b, and c are individual flags
   No dashes allowed inside
   
Examples:
 
        -vg is equivalent to -v -g
        -abc expands to three flags
Flags and parameters in one group:

If the last character in the group is a parameter (takes a value), it must be last:

        Value via separate arg: myprog mycmd -vo path
        
#### Free-form Arguments – Values Passed to Parameters or as Positional Arguments
   Any string (including special characters like /, :, . or even -) is allowed as a value to a parameter or as a positional argument, provided it's not interpreted as a flag or command.
   
   Examples:

        myprog clone https://example.com/repo.git
        myprog -o ./build/output.bin
        myprog run --script ./test.sh
   These are interpreted based on context - e.g., after -o, any value is accepted.

#### Short Parameter with Equals
After form two-letter short form is '=' and any string
Example:

        -o=~/path/to/myfile.ext

#### Long Parameter with Equals
After two-dashed long form is '=' and any string

Example:

        -ooutput=~/path/to/myfile.ext

#### Compact Flag Grouping with Parameter as Last with Equals
After compact form is '=' and any string. Note: If is n chars grouped together,
all n-1 must be flags, last must be parameter

Example:

        -vo=~/path/to/myfile.ext 
        (equivalent to --verbose --output=~/path/to/myfile.ext)


### Declaring Optional, Required, and Defaulted Parameters

This library provides three distinct methods for declaring parameters (key–value pairs) depending on how 
strictly they must appear on the command line:

#### addParameter(...) – Optional Parameter
Registers an optional parameter — one that may be provided by the user, but is not required.

If the parameter is missing, the associated value is not set.

Example:

    cmd.addParameter("--output", "-o", "path", "description for help");

    Use case: the output file must be type of path or may not be specified.

#### addReqParameter(...) – Required Parameter
Declares a required parameter — the program will fail if the user does not provide it.

Ideal for parameters like --config or --input that are mandatory.

Example:

    cmd.addReqParameter("--config","","path", "description for help");
    (note: second parameter = shorthand is not provided)

#### addDefParameter(...) – Parameter with Default Value
Defines a parameter that has a default value if it is not present on the command line.

Note: This does not mean the parameter is provided without an argument.
It means the parameter can be omitted entirely, and its value will default internally.

Typical for parameters like --directory where . (current directory) is a reasonable fallback.

Example:

    cmd.addDefParameter("--directory", "", "path", ".", "description for help");

### Option vs Flag vs Parameter
In this library, the term option is used in a broad sense — it refers to both flags and parameters.

However, for clarity and precision, it's helpful to distinguish between the two:
```
Concept    Example                          Has Value?  Description
Flag       --verbose, -v                    ❌           A boolean switch: either present or not.
Parameter  --output file.txt, -o file.txt   ✅           An option that requires a value (e.g. a path, number, string).
```
Terminology
* Flags are binary — they only indicate presence.
* Parameters carry additional data.
* Both are considered options at the syntactic level (they start with - or --), but behave differently semantically.

Example (Code)
```
cli::Application app("mycli");
    app.addCommand("build")
        .addFlag("--release", "-r", "Enable release mode")
        .addParameter("--output", "-o", "path", "Set output file path")
        .handler([](const cli::Actual* a) {
            if (a->containsFlag("--release")) {
                std::cout << "Release mode ON\n";
            }
            if (auto out = a->getValue("--output")) {
                std::cout << "Output: " << *out << "\n";
            }
        });
```
CLI Usage
```
mycli build -r -o bin/app

-r → sets --release flag.

-o bin/app → provides value for --output.
```
This separation improves expressiveness and avoids ambiguity, especially in complex CLI interfaces.

### Shared and Overridden Options

This library allows you to define global options (flags or parameters that apply to all commands) as well 
as command-specific options. For flexibility and clarity, it supports a clean overriding mechanism.

#### Global Options (Application-level)

You can define shared flags or parameters directly on the application. 
These will be available in all commands, unless explicitly overridden.

app.addFlag("--verbose", "-v", "Enable verbose output");

app.addParameter("--config", "-c", "path", "Path to configuration file");

Global options are ideal for reusable settings (verbosity, config paths, environments, etc.).

#### Command-level Overrides (Shadowing)

A command can override a global option by declaring an option with the same long and short name. 
This is useful when a command needs a specialized version of the same flag or parameter.
Rules for shadowing:
* Only flags can override flags, and parameters override parameters.
* The long name must match the global one exactly.
* The short name (shorthand) must also match if it was defined globally.

When shadowing:
* The description help string (desc) is optional
    * if is "" command has the same as global
    * can be overriden
* The description string (desc) is required, even if it's just "".
    * This makes it clear that a local override is intentional.
    * Helps avoid accidental shadowing.
* You may change the type (e.g., from required to defaulted).
* The command-local version fully replaces the global one in that command.

Example
```c++
app.addParameter("--output", "-o", "path", "Global output path");
app.addCommand("build")
.addDefParameter("--output", "-o", "path", ".", "") // override with no help and default "."
.handler([](const cli::Actual* a) {
std::cout << "Output: " << *a->getValue("--output") << "\n";
});
```

    --output is available globally, unless overridden.

    In build, we override it without help text, for command help will the same as global desc

    This allows full control per-command while keeping global defaults.

Tip: Share first, override when needed

You can define most options globally (e.g., --config, --verbose) as optional or defaulted. 
Then, when a command needs more control, override it locally.

In most cases use "" as a description for local overrides, it will be copied from global.

## Special Handling of `--help`

Users are accustomed to calling `--help` instead of command `help` as an option to display usage information, 
especially in CLI tools modeled after `gcc`. 
To support this expectation, the library defines `--help` in ways:

- alone or hve parameter --all 
- after command : gives help for this command
- before command : gives help for this command

**Normally, it is forbidden** to use the same name for both a flag and a parameter. 
However, `--help` is a special exception and the only case where this overlap is allowed.

Unlike many tools, this library **does not register `-h` as an alias for `--help`**, to avoid interfering 
with other potential uses of `-h` in specific commands.

Additionally:

- When `--help` is used **together with command**, `--help` takes precedence and suppresses normal execution.

## Expected Value Types

When declaring a parameter or positional argument, you can associate it with one or more expected types.

This enables automatic validation and clearer usage documentation.

Parameters
```c++
cmd.addParameter("--output", "-o", "path", "output file name");         // required path
cmd.addDefParameter("--output", "-o", "path", ".", "output file name"); // optional path with default value
```
    The parser will check that the value of --output matches the "path" validator.
    If omitted and a default is defined, that value is used instead.

Positional Arguments
```c++
cmd.addArg("threads", "int");
```
Multiple Types
```c++
cmd.addArg("repository", "url path");
```
This accepts either a URL or a local path.
Useful for commands like clone, where both forms are supported.

Example
```c++
.addCommand("clone")
.desc("Clone a repository from a URL or local path")
.addArg("repository", "url path")
.addArgs("directory", "path", 0, 1);
```
```shell
  mycli clone https://github.com/user/repo.git     # valid URL
  mycli clone ../local/repo                        # valid path
  mycli clone https://github.com/user/repo.git mydir # variable parameters from 0 to 1 times
```

Built-in Validator Types

You can use or extend the following types:
* "string" - any string
* "path", "linux-path", "windows-path", "auto-path"
* "url"
* "int", "float"

You can register your own validators using ValidatorManager

💬 Summary

    Types are declared in the addParameter(...), addReqParameter(...), addDefParameter(...) 
    or addArg(...), addArgs(...) methods.

    Multiple types can be specified as space-separated strings.

    This enables validation, better help messages, and improved UX (User Experience).
