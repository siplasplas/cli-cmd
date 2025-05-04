#ifdef CLICMD_HEADER_ONLY
#include "cli-cmd.hpp"
#else
#include "cli-cmd.h"
#endif

void clone_(cli::Application*, cli::Command* command)
{
    std::cout << "clone " << command->positionalArgs[0] << std::endl;
}

void addStartSubcategory(cli::Category *category)
{
    auto sub = category->addSubcategory("start a working area (see also: git help tutorial)");
    auto cmd = sub->addSubcomand(clone_, "clone", "Clone a repository into a new directory");
    cmd->setPositionalArgsLimits(1, 1);
    sub->addSubcomand(nullptr, "init", "Create an empty Git repository or reinitialize an existing one");
}

void addWorkSubcategory(cli::Category *category)
{
    auto sub = category->addSubcategory("work on the current change (see also: git help everyday)");
    sub->addSubcomand(nullptr, "add", "Add file contents to the index");
    sub->addSubcomand(nullptr, "mv", "Move or rename a file, a directory, or a symlink");
    sub->addSubcomand(nullptr, "restore", "Restore working tree files");
    sub->addSubcomand(nullptr, "rm", "Remove files from the working tree and from the index");
}

void addExamineSubcategory(cli::Category *category)
{
    auto sub = category->addSubcategory("examine the history and state (see also: git help revisions)");
    sub->addSubcomand(nullptr, "bisect", "Use binary search to find the commit that introduced a bug");
    sub->addSubcomand(nullptr, "diff", "Show changes between commits, commit and working tree, etc");
    sub->addSubcomand(nullptr,"grep" , "Print lines matching a pattern");
    sub->addSubcomand(nullptr, "log", "Show commit logs");
    sub->addSubcomand(nullptr, "show", "Show various types of objects");
    sub->addSubcomand(nullptr, "status", "Show the working tree status");
}

void addGrowSubcategory(cli::Category *category)
{
    auto sub = category->addSubcategory("grow, mark and tweak your common history");
    sub->addSubcomand(nullptr, "branch", "List, create, or delete branches");
    sub->addSubcomand(nullptr, "commit", "Record changes to the repository");
    sub->addSubcomand(nullptr, "merge", "Join two or more development histories together");
    sub->addSubcomand(nullptr, "rebase", "Reapply commits on top of another base tip");
    sub->addSubcomand(nullptr, "reset", "Reset current HEAD to the specified state");
    sub->addSubcomand(nullptr, "switch", "Switch branches");
    sub->addSubcomand(nullptr, "tag", "Create, list, delete or verify a tag object signed with GPG");
}

void addCollaborateSubcategory(cli::Category *category)
{
    auto sub = category->addSubcategory("collaborate (see also: git help workflows)");
    sub->addSubcomand(nullptr, "fetch", "Download objects and refs from another repository");
    sub->addSubcomand(nullptr, "pull", "Fetch from and integrate with another repository or a local branch");
    sub->addSubcomand(nullptr, "push", "Update remote refs along with associated objects");
}

void addPorcelainCommands(cli::Application &app)
{
    auto category = app.addCategory("Main Porcelain Commands");
    addStartSubcategory(category);
    addWorkSubcategory(category);
    addExamineSubcategory(category);
    addGrowSubcategory(category);
    addCollaborateSubcategory(category);
    category->addSubcomand(nullptr, "am", "Apply a series of patches from a mailbox");
    category->addSubcomand(nullptr, "archive", "Create an archive of files from a named tree");
    category->addSubcomand(nullptr, "bundle", "Move objects and refs by archive");
    category->addSubcomand(nullptr, "cherry-pick", "Apply the changes introduced by some existing commits");
    category->addSubcomand(nullptr, "citool", "Graphical alternative to git-commit");
    category->addSubcomand(nullptr, "clean", "Remove untracked files from the working tree");
    category->addSubcomand(nullptr, "describe", "Give an object a human readable name based on an available ref");
    category->addSubcomand(nullptr, "format-patch", "Prepare patches for e-mail submission");
    category->addSubcomand(nullptr, "gc", "Cleanup unnecessary files and optimize the local repository");
    category->addSubcomand(nullptr, "gitk", "The Git repository browser");
    category->addSubcomand(nullptr, "gui", "A portable graphical interface to Git");
    category->addSubcomand(nullptr, "maintenance", "Run tasks to optimize Git repository data");
    category->addSubcomand(nullptr, "notes", "Add or inspect object notes");
    category->addSubcomand(nullptr, "range-diff", "Compare two commit ranges (e.g. two versions of a branch)");
    category->addSubcomand(nullptr, "revert", "Revert some existing commits");
    category->addSubcomand(nullptr, "scalar", "A tool for managing large Git repositories");
    category->addSubcomand(nullptr, "shortlog", "Summarize 'git log' output");
    category->addSubcomand(nullptr, "sparse-checkout", "Reduce your working tree to a subset of tracked files");
    category->addSubcomand(nullptr, "stash", "Stash the changes in a dirty working directory away");
    category->addSubcomand(nullptr, "submodule", "Initialize, update or inspect submodules");
    category->addSubcomand(nullptr, "worktree", "Manage multiple working trees");
}


