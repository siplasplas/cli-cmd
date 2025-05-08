#ifdef CLICMD_HEADER_ONLY
#include "cli-cmd.hpp"
#else
#include "cli-cmd.h"
#endif

void addManipulators(cli::Application &app)
{
    auto category = app.addCategory("Ancillary Commands / Manipulators");
    category->addCommand("config").desc("Get and set repository or global options");
    category->addCommand("fast-export").desc("Git data exporter");
    category->addCommand("fast-import").desc("Backend for fast Git data importers");
    category->addCommand("filter-branch").desc("Rewrite branches");
    category->addCommand("mergetool").desc("Run merge conflict resolution tools to resolve merge conflicts");
    category->addCommand("pack-refs").desc("Pack heads and tags for efficient repository access");
    category->addCommand("prune").desc("Prune all unreachable objects from the object database");
    category->addCommand("reflog").desc("Manage reflog information");
    category->addCommand("remote").desc("Manage set of tracked repositories");
    category->addCommand("repack").desc("Pack unpacked objects in a repository");
    category->addCommand("replace").desc("Create, list, delete refs to replace objects");
}