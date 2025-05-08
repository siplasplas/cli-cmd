#include "cli-cmd.hpp"

void clone_(cli::Application*, cli::Command* commad)
{
    std::cout << "hello from handler!";
    std::cout << "   mycmd has " << commad->arguments.size() << " positional arguments"
        << std::endl;
}

int main(int argc, char** argv) {
    cli::Application app("first");
    app.addCommand("clone").desc("Clone a repository into a new directory").handler(clone_)
            .addArg("repository", "url").addArgs("directory", "path", 0, 1);
    app.run(argc, argv);
    return 0;
}
