#ifdef CLICMD_HEADER_ONLY
#include "cli-cmd.hpp"
#else
#include "cli-cmd.h"
#endif

void addStartSubcategory(cli::Category *category)
{
    auto sub = category->addSubcategory("start a working area (see also: git help tutorial)");
    auto cmd = sub->addCommand("clone", "Clone a repository into a new directory")
        .addArg("repository", "url");
    sub->addCommand("init", "Create an empty Git repository or reinitialize an existing one");
}

void addWorkSubcategory(cli::Category *category)
{
    auto sub = category->addSubcategory("work on the current change (see also: git help everyday)");
    sub->addCommand("add", "Add file contents to the index");
    sub->addCommand("mv", "Move or rename a file, a directory, or a symlink");
    sub->addCommand("restore", "Restore working tree files");
    sub->addCommand("rm", "Remove files from the working tree and from the index");
}

void addExamineSubcategory(cli::Category *category)
{
    auto sub = category->addSubcategory("examine the history and state (see also: git help revisions)");
    sub->addCommand("bisect", "Use binary search to find the commit that introduced a bug");
    sub->addCommand("diff", "Show changes between commits, commit and working tree, etc");
    sub->addCommand("grep" , "Print lines matching a pattern");
    sub->addCommand("log", "Show commit logs");
    sub->addCommand("show", "Show various types of objects");
    sub->addCommand("status", "Show the working tree status");
}

void addGrowSubcategory(cli::Category *category)
{
    auto sub = category->addSubcategory("grow, mark and tweak your common history");
    sub->addCommand("branch", "List, create, or delete branches");
    sub->addCommand("commit", "Record changes to the repository");
    sub->addCommand("merge", "Join two or more development histories together");
    sub->addCommand("rebase", "Reapply commits on top of another base tip");
    sub->addCommand("reset", "Reset current HEAD to the specified state");
    sub->addCommand("switch", "Switch branches");
    sub->addCommand("tag", "Create, list, delete or verify a tag object signed with GPG");
}

void addCollaborateSubcategory(cli::Category *category)
{
    auto sub = category->addSubcategory("collaborate (see also: git help workflows)");
    sub->addCommand("fetch", "Download objects and refs from another repository");
    sub->addCommand("pull", "Fetch from and integrate with another repository or a local branch");
    sub->addCommand("push", "Update remote refs along with associated objects");
}

void addPorcelainCommands(cli::Application &app)
{
    auto category = app.addCategory("Main Porcelain Commands");
    addStartSubcategory(category);
    addWorkSubcategory(category);
    addExamineSubcategory(category);
    addGrowSubcategory(category);
    addCollaborateSubcategory(category);
    category->addCommand("am", "Apply a series of patches from a mailbox");
    category->addCommand("archive", "Create an archive of files from a named tree");
    category->addCommand("bundle", "Move objects and refs by archive");
    category->addCommand("cherry-pick", "Apply the changes introduced by some existing commits");
    category->addCommand("citool", "Graphical alternative to git-commit");
    category->addCommand("clean", "Remove untracked files from the working tree");
    category->addCommand("describe", "Give an object a human readable name based on an available ref");
    category->addCommand("format-patch", "Prepare patches for e-mail submission");
    category->addCommand("gc", "Cleanup unnecessary files and optimize the local repository");
    category->addCommand("gitk", "The Git repository browser");
    category->addCommand("gui", "A portable graphical interface to Git");
    category->addCommand("maintenance", "Run tasks to optimize Git repository data");
    category->addCommand("notes", "Add or inspect object notes");
    category->addCommand("range-diff", "Compare two commit ranges (e.g. two versions of a branch)");
    category->addCommand("revert", "Revert some existing commits");
    category->addCommand("scalar", "A tool for managing large Git repositories");
    category->addCommand("shortlog", "Summarize 'git log' output");
    category->addCommand("sparse-checkout", "Reduce your working tree to a subset of tracked files");
    category->addCommand("stash", "Stash the changes in a dirty working directory away");
    category->addCommand("submodule", "Initialize, update or inspect submodules");
    category->addCommand("worktree", "Manage multiple working trees");
}


