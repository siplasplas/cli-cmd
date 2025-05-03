#include "cli-cmd.hpp"

void addPorcelainCommands(cli::Application &app);
void addManipulators(cli::Application &app);
void addInterrogators(cli::Application &app);
void addInteractingCommands(cli::Application &app);
void addLowLevelManipulators(cli::Application &app);
void addLowLevelInterrogators(cli::Application &app);
void addLowLevelSyncing(cli::Application &app);
void addLovLevelInternal(cli::Application &app);
void adduserFacing(cli::Application &app);
void addDeveloperFacing(cli::Application &app);
void addExternalCommands(cli::Application &app);

void show_version() {
    std::cout << "version" << std::endl;
}

class LikeGitApp: public cli::Application
{
public:
    LikeGitApp():cli::Application("likegit"){}
    void initHelp()
    {
        auto cmd = addSubcomand(
        [this](cli::Application* app, cli::Command* cmd) { this->help(app, cmd); },
        "help",
        "Display help information about likegit"
    );
        cmd->setPositionalArgsLimits(0, 1);
    }
};

int main() {
    LikeGitApp app;
    app.initHelp();
    //auto cmd = app.addSubcomand(help, "help", "Display help information about likegit"))
    //cmd->setPositionalArgsLimits(0, 1);
    addPorcelainCommands(app);
    addManipulators(app);
    addInterrogators(app);
    addInteractingCommands(app);
    addLowLevelManipulators(app);
    addLowLevelInterrogators(app);
    addLowLevelSyncing(app);
    addLovLevelInternal(app);
    adduserFacing(app);
    addDeveloperFacing(app);
    addExternalCommands(app);
    app.parse({"likegit","help"});
    return 0;
}
