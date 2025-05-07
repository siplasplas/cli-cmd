#include "cli-cmd.hpp"

void mycommand(cli::Application*, cli::Command* commad)
{
    std::cout << "hello from handler!";
    std::cout << "   mycmd has " << commad->positionalArgs.size() << " positional arguments"
        << std::endl;
}

int main(int argc, char** argv) {
    cli::Application app("first");
    auto cmd = app.addSubcomand(mycommand, "mycmd", "My example command");
    cmd->setPositionalArgsLimits(0, 1);
    app.run(argc, argv);
    return 0;
}
