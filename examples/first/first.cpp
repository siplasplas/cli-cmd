#include "cli-cmd.hpp"

class MyApp: public cli::Application
{
public:
    MyApp():cli::Application("MyApp"){}
    void initHelp()
    {
        addSubcomand(
        [this](cli::Application* app, cli::Command* cmd) { this->help(app, cmd); },
        "help","Display help information about likegit"
    );
    }
};


void mycommand(cli::Application*, cli::Command*)
{
    std::cout << "hello from handler!" << std::endl;
}

void addSubcategory(cli::Category *category)
{
    auto sub = category->addSubcategory("only common commands");
        auto cmd = sub->addSubcomand(mycommand, "mycmd", "Info about command");
    cmd->setPositionalArgsLimits(0, 1);
}

void addCategory(cli::Application &app)
{
    auto category = app.addCategory("Main category");
    addSubcategory(category);
}

int main(int argc, char** argv) {
    MyApp app;
    app.initHelp();
    addCategory(app);
    app.run(argc, argv);
    return 0;
}
