#ifdef CLICMD_HEADER_ONLY
#include "cli-cmd.hpp"
#else
#include "cli-cmd.h"
#endif

void addInteractingCommands(cli::Application &app)
{
    auto category = app.addCategory("Interacting with Others");
    category->addSubcomand(nullptr, "archimport", "Import a GNU Arch repository into Git");
    category->addSubcomand(nullptr, "cvsexportcommit", "Export a single commit to a CVS checkout");
    category->addSubcomand(nullptr, "cvsimport", "Salvage your data out of another SCM people love to hate");
    category->addSubcomand(nullptr, "cvsserver", "A CVS server emulator for Git");
    category->addSubcomand(nullptr, "imap-send", "Send a collection of patches from stdin to an IMAP folder");
    category->addSubcomand(nullptr, "p4", "Import from and submit to Perforce repositories");
    category->addSubcomand(nullptr, "quiltimport", "Applies a quilt patchset onto the current branch");
    category->addSubcomand(nullptr, "request-pull", "Generates a summary of pending changes");
    category->addSubcomand(nullptr, "send-email", "Send a collection of patches as emails");
    category->addSubcomand(nullptr, "svn", "Bidirectional operation between a Subversion repository and Git");
}

