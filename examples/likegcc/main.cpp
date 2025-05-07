#ifdef CLICMD_HEADER_ONLY
#include "cli-cmd.hpp"
#else
#include "cli-cmd.h"
#endif

void addOptions(cli::Application &app);
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

void show_version() {
    std::cout << "version" << std::endl;
}

int main(int argc, char** argv) {
    cli::Application app("likegcc");
    addOptions(app);
    app.run(argc, argv);
    return 0;
}
