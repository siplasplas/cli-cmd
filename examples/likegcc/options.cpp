#ifdef CLICMD_HEADER_ONLY
#include "cli-cmd.hpp"
#else
#include "cli-cmd.h"
#endif

void addOptions(cli::Command *cmd)
{
    cmd->addFlag("-pass-exit-codes", "", "Exit with highest error code from a phase.");
    cmd->addFlag("--help",  "", "Display this information.");
    cmd->addFlag("--target-help", "", "Display target specific command line options\n"
                                      "(including assembler and linker options).");
    cmd->addFlag("--version", "", "Display compiler version information.");
    cmd->addFlag("-dumpspecs", "", "Display all of the built in spec strings.");
    cmd->addFlag("-dumpversion", "", "Display the version of the compiler.");
    cmd->addFlag("-dumpmachine", "", "Display the compiler's target processor.");
    cmd->addParameter("-foffload", "", "Specify offloading targets.", "path");
    cmd->addFlag("-print-search-dirs", "", "Display the directories in the compiler's search path.");
    cmd->addFlag("-print-libgcc-file-name", "", "Display the name of the compiler's companion library.");
    cmd->addParameter("-print-file-name", "", "Display the full path to library <lib>.", "path");
    cmd->addParameter("-print-prog-name", "", "Display the full path to compiler component <prog>.", "path");
    cmd->addFlag("-print-multiarch", "", "Display the target's normalized GNU triplet, used as\n"
                                         "a component in the library path.");
    cmd->addFlag("-print-multi-directory", "", "Display the root directory for versions of libgcc.");
    cmd->addFlag("-print-multi-lib", "", "Display the mapping between command line options and\n"
                                        "multiple library search directories.");
    cmd->addFlag("-print-multi-os-directory", "", "Display the relative path to OS libraries.");
    cmd->addFlag("-print-sysroot", "", "Display the target libraries directory.");
    cmd->addFlag("-print-sysroot-headers-suffix", "", "Display the sysroot suffix used to find headers.");

    cmd->addParameter("-Wa", "", "Pass comma-separated <options> on to the assembler.", "string");
    cmd->addParameter("-Wp", "", "Pass comma-separated <options> on to the preprocessor.", "string");
    cmd->addParameter("-Wl", "", "Pass comma-separated <options> on to the linker.", "string");
    cmd->addParameter("-Xassembler", "", "Pass <arg> on to the assembler.", "string");
    cmd->addParameter("-Xpreprocessor", "", "Pass <arg> on to the preprocessor.", "string");
    cmd->addParameter("-Xlinker", "", "Pass <arg> on to the linker.", "string");
    cmd->addFlag("-save-temps", "", "Do not delete intermediate files.");
    cmd->addFlag("-no-canonical-prefixes", "", "Do not canonicalize paths when building relative\n"
                                               "prefixes to other gcc components.");
    cmd->addFlag("-pipe", "", "Use pipes rather than intermediate files.");
    cmd->addFlag("-time", "", "Time the execution of each subprocess.");
    cmd->addParameter("-specs", "", "Override built-in specs with the contents of <file>.", "path");
    cmd->addParameter("-std", "", "Assume that the input sources are for <standard>.", "string");
    cmd->addParameter("--sysroot", "", "Use <directory> as the root directory for headers\n"
                                       "and libraries.", "path");
    cmd->addParameter("-B", "", "Add <directory> to the compiler's search paths.", "path");
    cmd->addFlag("-v", "", "Display the programs invoked by the compiler.");
    cmd->addFlag("-E", "", "Preprocess only; do not compile, assemble or link.");
    cmd->addFlag("-S", "", "Compile only; do not assemble or link.");
    cmd->addFlag("-c", "", "Compile and assemble, but do not link.");
    cmd->addParameter("-o", "", "Place the output into <file>.", "path");
    cmd->addFlag("-pie", "", "Create a dynamically linked position independent\n"
                             "executable.");
    cmd->addFlag("-shared", "", "Create a shared library.");
    cmd->addParameter("-x", "", "Specify the language of the following input files.\n"
                                "Permissible languages include: c c++ assembler none\n"
                                "'none' means revert to the default behavior of\n"
                                "guessing the language based on the file's extension.", "string");
}
