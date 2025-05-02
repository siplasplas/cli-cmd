#include "../include/cli-cmd.hpp"

void show_version() {
    std::cout << "version" << std::endl;
}

int main() {
    cli::Application app("git");
    app.addSubcomand("show", show_version);
    app.addSubcomand("clone", show_version);
    app.addSubcomand("clean", show_version);
    app.addSubcomand("version", show_version);
    // test similarity
    app.parse({"git","clone"});
    app.parse({"git","lone"});
    app.parse({"git","andrzej"});
    app.parse({"git","clon1"});
    app.parse({"git","clo12"});
    app.parse({"git","cl123"});
    app.parse({"git","cl12"});
    app.parse({"git","-version"});
    app.parse({"git","-version"});
    return 0;
}
