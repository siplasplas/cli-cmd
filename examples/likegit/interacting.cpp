#ifdef CLICMD_HEADER_ONLY
#include "cli-cmd.hpp"
#else
#include "cli-cmd.h"
#endif

void addInteractingCommands(cli::Application &app)
{
    auto category = app.addCategory("Interacting with Others");
    category->addCommand("archimport").desc("Import a GNU Arch repository into Git");
    category->addCommand("cvsexportcommit").desc("Export a single commit to a CVS checkout");
    category->addCommand("cvsimport").desc("Salvage your data out of another SCM people love to hate");
    category->addCommand("cvsserver").desc("A CVS server emulator for Git");
    category->addCommand("imap-send").desc("Send a collection of patches from stdin to an IMAP folder");
    category->addCommand("p4").desc("Import from and submit to Perforce repositories");
    category->addCommand("quiltimport").desc("Applies a quilt patchset onto the current branch");
    category->addCommand("request-pull").desc("Generates a summary of pending changes");
    category->addCommand("send-email").desc("Send a collection of patches as emails");
    category->addCommand("svn").desc("Bidirectional operation between a Subversion repository and Git");
}

