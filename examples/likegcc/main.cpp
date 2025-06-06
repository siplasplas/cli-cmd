#ifdef CLICMD_HEADER_ONLY
#include "cli-cmd.hpp"
#else
#include "cli-cmd.h"
#endif
#include <iostream>

void addOptions(cli::Command *cmd);

void show_version() {
    std::cout << "version" << std::endl;
}

int mainHandler(const cli::Actual*)
{
    std::cout << "hello from main handler!";
    return 0;
}

int main(int argc, char** argv) {
    cli::Application app("likegcc", 0,0, 0);
    auto mainCommand = app.mainCommand.get();
    mainCommand->handler(mainHandler);
    addOptions(mainCommand);
    app.run(argc, argv);
    return 0;
}
