#ifdef CLICMD_HEADER_ONLY
#include "cli-cmd.hpp"
#else
#include "cli-cmd.h"
#endif

void addLowLevelInterrogators(cli::Application &app)
{
    auto category = app.addCategory("Low-level Commands / Interrogators");
    category->addSubcomand(nullptr, "cat-file", "Provide contents or details of repository objects");
    category->addSubcomand(nullptr, "cherry", "Find commits yet to be applied to upstream");
    category->addSubcomand(nullptr, "diff-files", "Compares files in the working tree and the index");
    category->addSubcomand(nullptr, "diff-index", "Compare a tree to the working tree or index");
    category->addSubcomand(nullptr, "diff-tree", "Compares the content and mode of blobs found via two tree objects");
    category->addSubcomand(nullptr, "for-each-ref", "Output information on each ref");
    category->addSubcomand(nullptr, "for-each-repo", "Run a Git command on a list of repositories");
    category->addSubcomand(nullptr, "get-tar-commit-id", "Extract commit ID from an archive created using git-archive");
    category->addSubcomand(nullptr, "ls-files", "Show information about files in the index and the working tree");
    category->addSubcomand(nullptr, "ls-remote", "List references in a remote repository");
    category->addSubcomand(nullptr, "ls-tree", "List the contents of a tree object");
    category->addSubcomand(nullptr, "merge-base", "Find as good common ancestors as possible for a merge");
    category->addSubcomand(nullptr, "name-rev", "Find symbolic names for given revs");
    category->addSubcomand(nullptr, "pack-redundant", "Find redundant pack files");
    category->addSubcomand(nullptr, "rev-list", "Lists commit objects in reverse chronological order");
    category->addSubcomand(nullptr, "rev-parse", "Pick out and massage parameters");
    category->addSubcomand(nullptr, "show-index", "Show packed archive index");
    category->addSubcomand(nullptr, "show-ref", "List references in a local repository");
    category->addSubcomand(nullptr, "unpack-file", "Creates a temporary file with a blob's contents");
    category->addSubcomand(nullptr, "var", "Show a Git logical variable");
    category->addSubcomand(nullptr, "verify-pack", "Validate packed Git archive files");
}
