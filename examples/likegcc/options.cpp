#ifdef CLICMD_HEADER_ONLY
#include "cli-cmd.hpp"
#else
#include "cli-cmd.h"
#endif

void clone_(cli::Application*, cli::Command* command)
{
    std::cout << "clone " << command->positionalArgs[0] << std::endl;
}

void addOneSubcategory(cli::Category *category)
{
    auto sub = category->addSubcategory("start a working area (see also: git help tutorial)");
    auto cmd = sub->addSubcomand(clone_, "clone", "Clone a repository into a new directory");
    cmd->setPositionalArgsLimits(1, 1);
    sub->addSubcomand(nullptr, "init", "Create an empty Git repository or reinitialize an existing one");
    sub->addOption("-pass-exit-codes","Exit with highest error code from a phase.");
    sub->addOption("--help",  "Display this information.");
    sub->addOption("--target-help", "Display target specific command line options\n"
                                    "(including assembler and linker options).");
    sub->addOption("--version", "Display compiler version information.");
    sub->addOption("-dumpspecs",  "Display all of the built in spec strings.");
    sub->addOption("-dumpversion", "Display the version of the compiler.");
    sub->addOption("-dumpmachine", "Display the compiler's target processor.");
    sub->addOption("-foffload=<targets>", "Specify offloading targets.");
    sub->addOption("-print-search-dirs", "Display the directories in the compiler's search path.");
    sub->addOption("-print-libgcc-file-name", "Display the name of the compiler's companion library.");
    sub->addOption("-print-file-name=<lib>",  "Display the full path to library <lib>.");
    sub->addOption("-print-prog-name=<prog>", "Display the full path to compiler component <prog>.");
    sub->addOption("-print-multiarch", "Display the target's normalized GNU triplet, used as\n"
                                                        "a component in the library path.");
    sub->addOption("-print-multi-directory", "Display the root directory for versions of libgcc.");
    sub->addOption("-print-multi-lib", "Display the mapping between command line options and\n"
                           "multiple library search directories.");
    sub->addOption("-print-multi-os-directory", "Display the relative path to OS libraries.");
    sub->addOption("-print-sysroot", "Display the target libraries directory.");
    sub->addOption("-print-sysroot-headers-suffix", "Display the sysroot suffix used to find headers.");
    sub->addOption("-Wa,<options>",  "Pass comma-separated <options> on to the assembler.");
    sub->addOption("-Wp,<options>",  "Pass comma-separated <options> on to the preprocessor.");
    sub->addOption("-Wl,<options>", "Pass comma-separated <options> on to the linker.");
    sub->addOption("-Xassembler <arg>", "Pass <arg> on to the assembler.");
    sub->addOption("-Xpreprocessor <arg>", "Pass <arg> on to the preprocessor.");
    sub->addOption("-Xlinker <arg>", "Pass <arg> on to the linker.");
    sub->addOption("-save-temps", "Do not delete intermediate files.");
    sub->addOption("-save-temps=<arg>", "Do not delete intermediate files.");
    sub->addOption("-no-canonical-prefixes", "Do not canonicalize paths when building relative\n"
                               "prefixes to other gcc components.");
    sub->addOption("-pipe", "Use pipes rather than intermediate files.");
    sub->addOption("-time",  "Time the execution of each subprocess.");
    sub->addOption("-specs=<file>", "Override built-in specs with the contents of <file>.");
    sub->addOption("-std=<standard>", "Assume that the input sources are for <standard>.");
    sub->addOption("--sysroot=<directory>", "Use <directory> as the root directory for headers\n"
                               "and libraries.");
    sub->addOption("-B <directory>", "Add <directory> to the compiler's search paths.");
    sub->addOption("-v", "Display the programs invoked by the compiler.");
    sub->addOption("-###", "Like -v but options quoted and commands not executed.");
    sub->addOption("-E", "Preprocess only; do not compile, assemble or link.");
    sub->addOption("-S", "Compile only; do not assemble or link.");
    sub->addOption("-c", "Compile and assemble, but do not link.");
    sub->addOption("-o <file>", "Place the output into <file>.");
    sub->addOption("-pie", "Create a dynamically linked position independent\n"
                               "executable.");
    sub->addOption("-shared", "Create a shared library.");
    sub->addOption("-x <language>", "Specify the language of the following input files.\n"
                               "Permissible languages include: c c++ assembler none\n"
                               "'none' means revert to the default behavior of\n"
                               "guessing the language based on the file's extension.");
}

void addOptions(cli::Application &app)
{
    auto category = app.addCategory("Main Porcelain Commands");
    addOneSubcategory(category);
}
