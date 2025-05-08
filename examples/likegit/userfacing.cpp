#ifdef CLICMD_HEADER_ONLY
#include "cli-cmd.hpp"
#else
#include "cli-cmd.h"
#endif

void adduserFacing(cli::Application &app)
{
    auto category = app.addCategory("User-facing repository, command and file interfaces");
    category->addCommand("attributes").desc( "Defining attributes per path");
    category->addCommand("cli").desc("Git command-line interface and conventions");
    category->addCommand("hooks").desc("Hooks used by Git");
    category->addCommand("ignore").desc("Specifies intentionally untracked files to ignore");
    category->addCommand("mailmap").desc("Map author/committer names and/or E-Mail addresses");
    category->addCommand("modules").desc("Defining submodule properties");
    category->addCommand("repository-layout").desc("Git Repository Layout");
    category->addCommand("revisions").desc("Specifying revisions and ranges for Git");
}
