#ifdef CLICMD_HEADER_ONLY
#include "cli-cmd.hpp"
#else
#include "cli-cmd.h"
#endif

void adduserFacing(cli::Application &app)
{
    auto category = app.addCategory("User-facing repository, command and file interfaces");
    category->addCommand("attributes", "Defining attributes per path");
    category->addCommand("cli", "Git command-line interface and conventions");
    category->addCommand("hooks", "Hooks used by Git");
    category->addCommand("ignore", "Specifies intentionally untracked files to ignore");
    category->addCommand("mailmap", "Map author/committer names and/or E-Mail addresses");
    category->addCommand("modules", "Defining submodule properties");
    category->addCommand("repository-layout", "Git Repository Layout");
    category->addCommand("revisions", "Specifying revisions and ranges for Git");
}
