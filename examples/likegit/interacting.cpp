#ifdef CLICMD_HEADER_ONLY
#include "cli-cmd.hpp"
#else
#include "cli-cmd.h"
#endif

void addInteractingCommands(cli::Application &app)
{
    auto category = app.addCategory("Interacting with Others");
    category->addCommand("archimport", "Import a GNU Arch repository into Git");
    category->addCommand("cvsexportcommit", "Export a single commit to a CVS checkout");
    category->addCommand("cvsimport", "Salvage your data out of another SCM people love to hate");
    category->addCommand("cvsserver", "A CVS server emulator for Git");
    category->addCommand("imap-send", "Send a collection of patches from stdin to an IMAP folder");
    category->addCommand("p4", "Import from and submit to Perforce repositories");
    category->addCommand("quiltimport", "Applies a quilt patchset onto the current branch");
    category->addCommand("request-pull", "Generates a summary of pending changes");
    category->addCommand("send-email", "Send a collection of patches as emails");
    category->addCommand("svn", "Bidirectional operation between a Subversion repository and Git");
}

