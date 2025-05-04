#ifdef CLICMD_HEADER_ONLY
#include "cli-cmd.hpp"
#else
#include "cli-cmd.h"
#endif

void addInterrogators(cli::Application &app)
{
    auto category = app.addCategory("Ancillary Commands / Interrogators");
    category->addSubcomand(nullptr, "annotate", "Annotate file lines with commit information");
    category->addSubcomand(nullptr, "blame", "Show what revision and author last modified each line of a file");
    category->addSubcomand(nullptr, "bugreport", "Collect information for user to file a bug report");
    category->addSubcomand(nullptr, "count-objects", "Count unpacked number of objects and their disk consumption");
    category->addSubcomand(nullptr, "diagnose", "Generate a zip archive of diagnostic information");
    category->addSubcomand(nullptr, "difftool", "Show changes using common diff tools");
    category->addSubcomand(nullptr, "fsck", "Verifies the connectivity and validity of the objects in the database");
    category->addSubcomand(nullptr, "gitweb", "Git web interface (web frontend to Git repositories)");
    category->addSubcomand(nullptr, "instaweb", "Instantly browse your working repository in gitweb");
    category->addSubcomand(nullptr, "merge-tree", "Perform merge without touching index or working tree");
    category->addSubcomand(nullptr, "rerere", "Reuse recorded resolution of conflicted merges");
    category->addSubcomand(nullptr, "show-branch", "Show branches and their commits");
    category->addSubcomand(nullptr, "verify-commit", "Check the GPG signature of commits");
    category->addSubcomand(nullptr, "verify-tag", "Check the GPG signature of tags");
    category->addSubcomand(nullptr, "version", "Display version information about Git");
    category->addSubcomand(nullptr, "whatchanged", "Show logs with differences each commit introduces");
}

