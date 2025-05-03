#ifdef CLICMD_HEADER_ONLY
#include "cli-cmd.hpp"
#else
#include "cli-cmd.h"
#endif

void addStartSubcategory(cli::Category *category)
{
    category->addSubcategory("start a working area (see also: git help tutorial)");
}

void addWorkSubcategory(cli::Category *category)
{
    category->addSubcategory("work on the current change (see also: git help everyday)");
}

void addExamineSubcategory(cli::Category *category)
{
    category->addSubcategory("examine the history and state (see also: git help revisions)");
}

void addGrowSubcategory(cli::Category *category)
{
    category->addSubcategory("grow, mark and tweak your common history");
}

void addCollaborateSubcategory(cli::Category *category)
{
    category->addSubcategory("collaborate (see also: git help workflows)");
}

void addPorcelainCommands(cli::Application &app)
{
    auto category = app.addCategory("Main Porcelain Commands");
    addStartSubcategory(category);
    addWorkSubcategory(category);
    addExamineSubcategory(category);
    addGrowSubcategory(category);
    addCollaborateSubcategory(category);
}
