#ifdef CLICMD_HEADER_ONLY
#include "cli-cmd.hpp"
#else
#include "cli-cmd.h"
#endif

void clone_(cli::Application*, cli::Command* command)
{
    std::cout << "clone " << command->positionalArgs[0] << std::endl;
}

void addOneSubcategory(cli::Category *category)
{
    auto sub = category->addSubcategory("start a working area (see also: git help tutorial)");
    auto cmd = sub->addSubcomand(clone_, "clone", "Clone a repository into a new directory");
    cmd->setPositionalArgsLimits(1, 1);
    sub->addSubcomand(nullptr, "init", "Create an empty Git repository or reinitialize an existing one");
}

void addOptions(cli::Application &app)
{
    auto category = app.addCategory("Main Porcelain Commands");
    addOneSubcategory(category);
}
