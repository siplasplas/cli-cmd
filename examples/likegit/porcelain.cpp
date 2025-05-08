#ifdef CLICMD_HEADER_ONLY
#include "cli-cmd.hpp"
#else
#include "cli-cmd.h"
#endif

void addStartSubcategory(cli::Category *category)
{
    auto sub = category->addSubcategory("start a working area (see also: git help tutorial)");
    auto cmd = sub->addCommand("clone").desc("Clone a repository into a new directory")
        .addArg("repository", "url");
    sub->addCommand("init").desc("Create an empty Git repository or reinitialize an existing one");
}

void addWorkSubcategory(cli::Category *category)
{
    auto sub = category->addSubcategory("work on the current change (see also: git help everyday)");
    sub->addCommand("add").desc("Add file contents to the index");
    sub->addCommand("mv").desc("Move or rename a file, a directory, or a symlink");
    sub->addCommand("restore").desc("Restore working tree files");
    sub->addCommand("rm").desc("Remove files from the working tree and from the index");
}

void addExamineSubcategory(cli::Category *category)
{
    auto sub = category->addSubcategory("examine the history and state (see also: git help revisions)");
    sub->addCommand("bisect").desc("Use binary search to find the commit that introduced a bug");
    sub->addCommand("diff").desc("Show changes between commits, commit and working tree, etc");
    sub->addCommand("grep").desc("Print lines matching a pattern");
    sub->addCommand("log").desc("Show commit logs");
    sub->addCommand("show").desc("Show various types of objects");
    sub->addCommand("status").desc("Show the working tree status");
}

void addGrowSubcategory(cli::Category *category)
{
    auto sub = category->addSubcategory("grow, mark and tweak your common history");
    sub->addCommand("branch").desc("List, create, or delete branches");
    sub->addCommand("commit").desc("Record changes to the repository");
    sub->addCommand("merge").desc("Join two or more development histories together");
    sub->addCommand("rebase").desc("Reapply commits on top of another base tip");
    sub->addCommand("reset").desc("Reset current HEAD to the specified state");
    sub->addCommand("switch").desc("Switch branches");
    sub->addCommand("tag").desc("Create, list, delete or verify a tag object signed with GPG");
}

void addCollaborateSubcategory(cli::Category *category)
{
    auto sub = category->addSubcategory("collaborate (see also: git help workflows)");
    sub->addCommand("fetch").desc("Download objects and refs from another repository");
    sub->addCommand("pull").desc("Fetch from and integrate with another repository or a local branch");
    sub->addCommand("push").desc("Update remote refs along with associated objects");
}

void addPorcelainCommands(cli::Application &app)
{
    auto category = app.addCategory("Main Porcelain Commands");
    addStartSubcategory(category);
    addWorkSubcategory(category);
    addExamineSubcategory(category);
    addGrowSubcategory(category);
    addCollaborateSubcategory(category);
    category->addCommand("am").desc("Apply a series of patches from a mailbox");
    category->addCommand("archive").desc("Create an archive of files from a named tree");
    category->addCommand("bundle").desc("Move objects and refs by archive");
    category->addCommand("cherry-pick").desc("Apply the changes introduced by some existing commits");
    category->addCommand("citool").desc("Graphical alternative to git-commit");
    category->addCommand("clean").desc("Remove untracked files from the working tree");
    category->addCommand("describe").desc("Give an object a human readable name based on an available ref");
    category->addCommand("format-patch").desc("Prepare patches for e-mail submission");
    category->addCommand("gc").desc("Cleanup unnecessary files and optimize the local repository");
    category->addCommand("gitk").desc("The Git repository browser");
    category->addCommand("gui").desc("A portable graphical interface to Git");
    category->addCommand("maintenance").desc("Run tasks to optimize Git repository data");
    category->addCommand("notes").desc("Add or inspect object notes");
    category->addCommand("range-diff").desc("Compare two commit ranges (e.g. two versions of a branch)");
    category->addCommand("revert").desc("Revert some existing commits");
    category->addCommand("scalar").desc("A tool for managing large Git repositories");
    category->addCommand("shortlog").desc("Summarize 'git log' output");
    category->addCommand("sparse-checkout").desc("Reduce your working tree to a subset of tracked files");
    category->addCommand("stash").desc("Stash the changes in a dirty working directory away");
    category->addCommand("submodule").desc("Initialize, update or inspect submodules");
    category->addCommand("worktree").desc("Manage multiple working trees");
}


