#ifdef CLICMD_HEADER_ONLY
#include "cli-cmd.hpp"
#else
#include "cli-cmd.h"
#endif

void addInterrogators(cli::Application &app)
{
    auto category = app.addCategory("Ancillary Commands / Interrogators");
    category->addCommand("annotate", "Annotate file lines with commit information");
    category->addCommand("blame", "Show what revision and author last modified each line of a file");
    category->addCommand("bugreport", "Collect information for user to file a bug report");
    category->addCommand("count-objects", "Count unpacked number of objects and their disk consumption");
    category->addCommand("diagnose", "Generate a zip archive of diagnostic information");
    category->addCommand("difftool", "Show changes using common diff tools");
    category->addCommand("fsck", "Verifies the connectivity and validity of the objects in the database");
    category->addCommand("gitweb", "Git web interface (web frontend to Git repositories)");
    category->addCommand("instaweb", "Instantly browse your working repository in gitweb");
    category->addCommand("merge-tree", "Perform merge without touching index or working tree");
    category->addCommand("rerere", "Reuse recorded resolution of conflicted merges");
    category->addCommand("show-branch", "Show branches and their commits");
    category->addCommand("verify-commit", "Check the GPG signature of commits");
    category->addCommand("verify-tag", "Check the GPG signature of tags");
    category->addCommand("version", "Display version information about Git");
    category->addCommand("whatchanged", "Show logs with differences each commit introduces");
}

