#include "cli-cmd.hpp"

void clone_(const cli::Actual* actual) {
    std::cout << "hello from handler!\n";
    std::cout << "   mycmd has " << actual->arguments.size() << " positional arguments\n";
}

int main(int argc, char** argv) {
    cli::Application app("first", 1, 1, 1); // cmdDepth, combineOpts, helpFlags
    app.addCommand("clone")
        .desc("Clone a repository into a new directory")
        .addArg("repository", "url")
        .addArgs("directory", "auto-path", 0, 1)
        .handler(clone_);
    app.run(argc, argv);
}
