#ifdef CLICMD_HEADER_ONLY
#include "cli-cmd.hpp"
#else
#include "cli-cmd.h"
#endif

void addManipulators(cli::Application &app)
{
    auto category = app.addCategory("Ancillary Commands / Manipulators");
    category->addSubcomand(nullptr, "config", "Get and set repository or global options");
    category->addSubcomand(nullptr, "fast-export", "Git data exporter");
    category->addSubcomand(nullptr, "fast-import", "Backend for fast Git data importers");
    category->addSubcomand(nullptr, "filter-branch", "Rewrite branches");
    category->addSubcomand(nullptr, "mergetool", "Run merge conflict resolution tools to resolve merge conflicts");
    category->addSubcomand(nullptr, "pack-refs", "Pack heads and tags for efficient repository access");
    category->addSubcomand(nullptr, "prune", "Prune all unreachable objects from the object database");
    category->addSubcomand(nullptr, "reflog", "Manage reflog information");
    category->addSubcomand(nullptr, "remote", "Manage set of tracked repositories");
    category->addSubcomand(nullptr, "repack", "Pack unpacked objects in a repository");
    category->addSubcomand(nullptr, "replace", "Create, list, delete refs to replace objects");
}