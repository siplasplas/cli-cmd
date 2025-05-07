#ifdef CLICMD_HEADER_ONLY
#include "cli-cmd.hpp"
#else
#include "cli-cmd.h"
#endif

void addOptions(cli::Application &app);

void show_version() {
    std::cout << "version" << std::endl;
}

int main(int argc, char** argv) {
    cli::Application app("likegcc", "cmdDepth=3");
    addOptions(app);
    app.run(argc, argv);
    return 0;
}
