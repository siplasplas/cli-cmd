#ifdef CLICMD_HEADER_ONLY
#include "cli-cmd.hpp"
#else
#include "cli-cmd.h"
#endif

void addLowLevelInterrogators(cli::Application &app)
{
    auto category = app.addCategory("Low-level Commands / Interrogators");
    category->addCommand("cat-file", "Provide contents or details of repository objects");
    category->addCommand("cherry", "Find commits yet to be applied to upstream");
    category->addCommand("diff-files", "Compares files in the working tree and the index");
    category->addCommand("diff-index", "Compare a tree to the working tree or index");
    category->addCommand("diff-tree", "Compares the content and mode of blobs found via two tree objects");
    category->addCommand("for-each-ref", "Output information on each ref");
    category->addCommand("for-each-repo", "Run a Git command on a list of repositories");
    category->addCommand("get-tar-commit-id", "Extract commit ID from an archive created using git-archive");
    category->addCommand("ls-files", "Show information about files in the index and the working tree");
    category->addCommand("ls-remote", "List references in a remote repository");
    category->addCommand("ls-tree", "List the contents of a tree object");
    category->addCommand("merge-base", "Find as good common ancestors as possible for a merge");
    category->addCommand("name-rev", "Find symbolic names for given revs");
    category->addCommand("pack-redundant", "Find redundant pack files");
    category->addCommand("rev-list", "Lists commit objects in reverse chronological order");
    category->addCommand("rev-parse", "Pick out and massage parameters");
    category->addCommand("show-index", "Show packed archive index");
    category->addCommand("show-ref", "List references in a local repository");
    category->addCommand("unpack-file", "Creates a temporary file with a blob's contents");
    category->addCommand("var", "Show a Git logical variable");
    category->addCommand("verify-pack", "Validate packed Git archive files");
}
