#ifdef CLICMD_HEADER_ONLY
#include "cli-cmd.hpp"
#else
#include "cli-cmd.h"
#endif

void addInterrogators(cli::Application &app)
{
    auto category = app.addCategory("Ancillary Commands / Interrogators");
    category->addCommand("annotate").desc("Annotate file lines with commit information");
    category->addCommand("blame").desc("Show what revision and author last modified each line of a file");
    category->addCommand("bugreport").desc("Collect information for user to file a bug report");
    category->addCommand("count-objects").desc("Count unpacked number of objects and their disk consumption");
    category->addCommand("diagnose").desc("Generate a zip archive of diagnostic information");
    category->addCommand("difftool").desc("Show changes using common diff tools");
    category->addCommand("fsck").desc("Verifies the connectivity and validity of the objects in the database");
    category->addCommand("gitweb").desc("Git web interface (web frontend to Git repositories)");
    category->addCommand("instaweb").desc("Instantly browse your working repository in gitweb");
    category->addCommand("merge-tree").desc("Perform merge without touching index or working tree");
    category->addCommand("rerere").desc("Reuse recorded resolution of conflicted merges");
    category->addCommand("show-branch").desc("Show branches and their commits");
    category->addCommand("verify-commit").desc("Check the GPG signature of commits");
    category->addCommand("verify-tag").desc("Check the GPG signature of tags");
    category->addCommand("version").desc("Display version information about Git");
    category->addCommand("whatchanged").desc("Show logs with differences each commit introduces");
}

