#ifdef CLICMD_HEADER_ONLY
#include "cli-cmd.hpp"
#else
#include "cli-cmd.h"
#endif
#include <iostream>

void addPorcelainCommands(cli::Application &app);
void addManipulators(cli::Application &app);
void addInterrogators(cli::Application &app);
void addInteractingCommands(cli::Application &app);
void addLowLevelManipulators(cli::Application &app);
void addLowLevelInterrogators(cli::Application &app);
void addLowLevelSyncing(cli::Application &app);
void addLovLevelInternal(cli::Application &app);
void adduserFacing(cli::Application &app);
void addDeveloperFacing(cli::Application &app);
void addExternalCommands(cli::Application &app);
void addCommonCommands(cli::Application &app);

void clone_(const cli::Actual* actual)
{
    std::cout << "clone " << actual->arguments[0].value << std::endl;
}

int main(int argc, char** argv) {
    cli::Application app("likegit", "cmdDepth=3 diagnostic=0");
    addPorcelainCommands(app);
    addManipulators(app);
    addInterrogators(app);
    addInteractingCommands(app);
    addLowLevelManipulators(app);
    addLowLevelInterrogators(app);
    addLowLevelSyncing(app);
    addLovLevelInternal(app);
    adduserFacing(app);
    addDeveloperFacing(app);
    addExternalCommands(app);
    addCommonCommands(app);
    auto cloneCmd = app.getCommand("clone");
    cloneCmd->handler(clone_);
    cloneCmd->addFlag("--verbose", "-v", "be more verbose");
    cloneCmd->addFlag("--quiet", "-q", "be more quiet");
    app.run(argc, argv);
    return 0;
}
