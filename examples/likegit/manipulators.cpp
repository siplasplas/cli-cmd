#ifdef CLICMD_HEADER_ONLY
#include "cli-cmd.hpp"
#else
#include "cli-cmd.h"
#endif

void addManipulators(cli::Application &app)
{
    auto category = app.addCategory("Ancillary Commands / Manipulators");
    category->addCommand("config", "Get and set repository or global options");
    category->addCommand("fast-export", "Git data exporter");
    category->addCommand("fast-import", "Backend for fast Git data importers");
    category->addCommand("filter-branch", "Rewrite branches");
    category->addCommand("mergetool", "Run merge conflict resolution tools to resolve merge conflicts");
    category->addCommand("pack-refs", "Pack heads and tags for efficient repository access");
    category->addCommand("prune", "Prune all unreachable objects from the object database");
    category->addCommand("reflog", "Manage reflog information");
    category->addCommand("remote", "Manage set of tracked repositories");
    category->addCommand("repack", "Pack unpacked objects in a repository");
    category->addCommand("replace", "Create, list, delete refs to replace objects");
}