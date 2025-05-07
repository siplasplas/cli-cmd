#ifdef CLICMD_HEADER_ONLY
#include "cli-cmd.hpp"
#else
#include "cli-cmd.h"
#endif

void addOptions(cli::Application &app);

void show_version() {
    std::cout << "version" << std::endl;
}

void mainHandler(cli::Application*, cli::Command*)
{
    std::cout << "hello from main handler!";
}

int main(int argc, char** argv) {
    cli::Application app("likegcc", "cmdDepth=0");
    auto mainCommand = app.mainCommand;
    mainCommand->setHandler(mainHandler);
    app.run(argc, argv);
    return 0;
}
