#ifdef CLICMD_HEADER_ONLY
#include "cli-cmd.hpp"
#else
#include "cli-cmd.h"
#endif

void addOptions(cli::Command *cmd)
{
    cmd->addOption("-pass-exit-codes","Exit with highest error code from a phase.");
    cmd->addOption("--help",  "Display this information.");
    cmd->addOption("--target-help", "Display target specific command line options\n"
                                    "(including assembler and linker options).");
    cmd->addOption("--version", "Display compiler version information.");
    cmd->addOption("-dumpspecs",  "Display all of the built in spec strings.");
    cmd->addOption("-dumpversion", "Display the version of the compiler.");
    cmd->addOption("-dumpmachine", "Display the compiler's target processor.");
    cmd->addOption("-foffload=<targets>", "Specify offloading targets.");
    cmd->addOption("-print-search-dirs", "Display the directories in the compiler's search path.");
    cmd->addOption("-print-libgcc-file-name", "Display the name of the compiler's companion library.");
    cmd->addOption("-print-file-name=<lib>",  "Display the full path to library <lib>.");
    cmd->addOption("-print-prog-name=<prog>", "Display the full path to compiler component <prog>.");
    cmd->addOption("-print-multiarch", "Display the target's normalized GNU triplet, used as\n"
                                                        "a component in the library path.");
    cmd->addOption("-print-multi-directory", "Display the root directory for versions of libgcc.");
    cmd->addOption("-print-multi-lib", "Display the mapping between command line options and\n"
                           "multiple library search directories.");
    cmd->addOption("-print-multi-os-directory", "Display the relative path to OS libraries.");
    cmd->addOption("-print-sysroot", "Display the target libraries directory.");
    cmd->addOption("-print-sysroot-headers-suffix", "Display the sysroot suffix used to find headers.");
    cmd->addOption("-Wa,<options>",  "Pass comma-separated <options> on to the assembler.");
    cmd->addOption("-Wp,<options>",  "Pass comma-separated <options> on to the preprocessor.");
    cmd->addOption("-Wl,<options>", "Pass comma-separated <options> on to the linker.");
    cmd->addOption("-Xassembler <arg>", "Pass <arg> on to the assembler.");
    cmd->addOption("-Xpreprocessor <arg>", "Pass <arg> on to the preprocessor.");
    cmd->addOption("-Xlinker <arg>", "Pass <arg> on to the linker.");
    cmd->addOption("-save-temps", "Do not delete intermediate files.");
    cmd->addOption("-save-temps=<arg>", "Do not delete intermediate files.");
    cmd->addOption("-no-canonical-prefixes", "Do not canonicalize paths when building relative\n"
                               "prefixes to other gcc components.");
    cmd->addOption("-pipe", "Use pipes rather than intermediate files.");
    cmd->addOption("-time",  "Time the execution of each subprocess.");
    cmd->addOption("-specs=<file>", "Override built-in specs with the contents of <file>.");
    cmd->addOption("-std=<standard>", "Assume that the input sources are for <standard>.");
    cmd->addOption("--sysroot=<directory>", "Use <directory> as the root directory for headers\n"
                               "and libraries.");
    cmd->addOption("-B <directory>", "Add <directory> to the compiler's search paths.");
    cmd->addOption("-v", "Display the programs invoked by the compiler.");
    cmd->addOption("-###", "Like -v but options quoted and commands not executed.");
    cmd->addOption("-E", "Preprocess only; do not compile, assemble or link.");
    cmd->addOption("-S", "Compile only; do not assemble or link.");
    cmd->addOption("-c", "Compile and assemble, but do not link.");
    cmd->addOption("-o <file>", "Place the output into <file>.");
    cmd->addOption("-pie", "Create a dynamically linked position independent\n"
                               "executable.");
    cmd->addOption("-shared", "Create a shared library.");
    cmd->addOption("-x <language>", "Specify the language of the following input files.\n"
                               "Permissible languages include: c c++ assembler none\n"
                               "'none' means revert to the default behavior of\n"
                               "guessing the language based on the file's extension.");
}
