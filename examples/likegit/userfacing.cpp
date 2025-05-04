#ifdef CLICMD_HEADER_ONLY
#include "cli-cmd.hpp"
#else
#include "cli-cmd.h"
#endif

void adduserFacing(cli::Application &app)
{
    auto category = app.addCategory("User-facing repository, command and file interfaces");
    category->addSubcomand(nullptr, "attributes", "Defining attributes per path");
    category->addSubcomand(nullptr, "cli", "Git command-line interface and conventions");
    category->addSubcomand(nullptr, "hooks", "Hooks used by Git");
    category->addSubcomand(nullptr, "ignore", "Specifies intentionally untracked files to ignore");
    category->addSubcomand(nullptr, "mailmap", "Map author/committer names and/or E-Mail addresses");
    category->addSubcomand(nullptr, "modules", "Defining submodule properties");
    category->addSubcomand(nullptr, "repository-layout", "Git Repository Layout");
    category->addSubcomand(nullptr, "revisions", "Specifying revisions and ranges for Git");
}
