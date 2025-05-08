#ifdef CLICMD_HEADER_ONLY
#include "cli-cmd.hpp"
#else
#include "cli-cmd.h"
#endif

void addLowLevelInterrogators(cli::Application &app)
{
    auto category = app.addCategory("Low-level Commands / Interrogators");
    category->addCommand("cat-file").desc("Provide contents or details of repository objects");
    category->addCommand("cherry").desc("Find commits yet to be applied to upstream");
    category->addCommand("diff-files").desc("Compares files in the working tree and the index");
    category->addCommand("diff-index").desc("Compare a tree to the working tree or index");
    category->addCommand("diff-tree").desc("Compares the content and mode of blobs found via two tree objects");
    category->addCommand("for-each-ref").desc("Output information on each ref");
    category->addCommand("for-each-repo").desc("Run a Git command on a list of repositories");
    category->addCommand("get-tar-commit-id").desc("Extract commit ID from an archive created using git-archive");
    category->addCommand("ls-files").desc("Show information about files in the index and the working tree");
    category->addCommand("ls-remote").desc("List references in a remote repository");
    category->addCommand("ls-tree").desc("List the contents of a tree object");
    category->addCommand("merge-base").desc("Find as good common ancestors as possible for a merge");
    category->addCommand("name-rev").desc("Find symbolic names for given revs");
    category->addCommand("pack-redundant").desc("Find redundant pack files");
    category->addCommand("rev-list").desc("Lists commit objects in reverse chronological order");
    category->addCommand("rev-parse").desc("Pick out and massage parameters");
    category->addCommand("show-index").desc("Show packed archive index");
    category->addCommand("show-ref").desc("List references in a local repository");
    category->addCommand("unpack-file").desc("Creates a temporary file with a blob's contents");
    category->addCommand("var").desc("Show a Git logical variable");
    category->addCommand("verify-pack").desc("Validate packed Git archive files");
}
