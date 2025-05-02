#include "../include/cli-cmd.hpp"

void show_version() {
    std::cout << "version" << std::endl;
}


int main() {
    cli::Application app("git");
    app.addSubcomand("--version", show_version);
    app.addSubcomand("--clone", show_version);
    app.addSubcomand("--show", show_version);
    app.parse({"git","--version"});
    return 0;
}
