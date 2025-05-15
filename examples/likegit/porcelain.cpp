#ifdef CLICMD_HEADER_ONLY
#include "cli-cmd.hpp"
#else
#include "cli-cmd.h"
#endif

void addCommonCommands(cli::Application &app)
{
    app.addHelpCategory("start a working area (see also: git help tutorial)")
        .ref("clone").ref("init");
    app.addHelpCategory("work on the current change (see also: git help everyday)")
        .ref("add").ref("mv").ref("restore").ref("rm");
    app.addHelpCategory("examine the history and state (see also: git help revisions)")
        .ref("bisect").ref("diff").ref("grep").ref("log").ref("show").ref("status");
    app.addHelpCategory("grow, mark and tweak your common history")
        .ref("branch").ref("commit").ref("merge").ref("rebase").ref("reset").ref("switch").ref("tag");
    app.addHelpCategory("collaborate (see also: git help workflows)")
        .ref("fetch").ref("pull").ref("push");
}

void addPorcelainCommands(cli::Application &app)
{
    auto category = app.addCategory("Main Porcelain Commands");
    category->addCommand("add").desc("Add file contents to the index");
    category->addCommand("am").desc("Apply a series of patches from a mailbox");
    category->addCommand("archive").desc("Create an archive of files from a named tree");
    category->addCommand("bisect").desc("Use binary search to find the commit that introduced a bug");
    category->addCommand("branch").desc("List, create, or delete branches");
    category->addCommand("bundle").desc("Move objects and refs by archive");
    category->addCommand("checkout").desc("Switch branches or restore working tree files");
    category->addCommand("cherry-pick").desc("Apply the changes introduced by some existing commits");
    category->addCommand("citool").desc("Graphical alternative to git-commit");
    category->addCommand("clean").desc("Remove untracked files from the working tree");
    category->addCommand("clone").desc("Clone a repository into a new directory")
        .addArg("repository","auto-path");
    category->addCommand("commit").desc("Record changes to the repository");
    category->addCommand("describe").desc("Give an object a human readable name based on an available ref");
    category->addCommand("diff").desc("Show changes between commits, commit and working tree, etc");
    category->addCommand("fetch").desc("Download objects and refs from another repository");
    category->addCommand("format-patch").desc("Prepare patches for e-mail submission");
    category->addCommand("gc").desc("Cleanup unnecessary files and optimize the local repository");
    category->addCommand("gitk").desc("The Git repository browser");
    category->addCommand("grep").desc("Print lines matching a pattern");
    category->addCommand("gui").desc("A portable graphical interface to Git");
    category->addCommand("init").desc("Create an empty Git repository or reinitialize an existing one");
    category->addCommand("log").desc("Show commit logs");
    category->addCommand("maintenance").desc("Run tasks to optimize Git repository data");
    category->addCommand("merge").desc("Join two or more development histories together");
    category->addCommand("mv").desc("Move or rename a file, a directory, or a symlink");
    category->addCommand("notes").desc("Add or inspect object notes");
    category->addCommand("pull").desc("Fetch from and integrate with another repository or a local branch");
    category->addCommand("push").desc("Update remote refs along with associated objects");
    category->addCommand("range-diff").desc("Compare two commit ranges (e.g. two versions of a branch)");
    category->addCommand("rebase").desc("Reapply commits on top of another base tip");
    category->addCommand("reset").desc("Reset current HEAD to the specified state");
    category->addCommand("restore").desc("Restore working tree files");
    category->addCommand("revert").desc("Revert some existing commits");
    category->addCommand("rm").desc("Remove files from the working tree and from the index");
    category->addCommand("scalar").desc("A tool for managing large Git repositories");
    category->addCommand("shortlog").desc("Summarize 'git log' output");
    category->addCommand("show").desc("Show various types of objects");
    category->addCommand("sparse-checkout").desc("Reduce your working tree to a subset of tracked files");
    category->addCommand("stash").desc("Stash the changes in a dirty working directory away");
    category->addCommand("status").desc("Show the working tree status");
    category->addCommand("submodule").desc("Initialize, update or inspect submodules");
    category->addCommand("switch").desc("Switch branches");
    category->addCommand("tag").desc("Create, list, delete or verify a tag object signed with GPG");
    category->addCommand("worktree").desc("Manage multiple working trees");
}


