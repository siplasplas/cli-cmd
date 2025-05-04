#include "cli-cmd.hpp"

class MyApp: public cli::Application
{
public:
    MyApp():cli::Application("MyApp"){}
    void initHelp()
    {
        addSubcomand(
        [this](cli::Application* app, cli::Command* cmd) { this->help(app, cmd); },
        "help","Display help information about MyApp"
    );
    }
};

void mycommand(cli::Application*, cli::Command* commad)
{
    std::cout << "hello from handler!";
    std::cout << "   mycmd has " << commad->positionalArgs.size() << " positional arguments"
        << std::endl;
}

int main(int argc, char** argv) {
    MyApp app;
    app.initHelp();
    auto cmd = app.addSubcomand(mycommand, "mycmd", "Info about command");
    cmd->setPositionalArgsLimits(0, 1);
    app.run(argc, argv);
    return 0;
}
