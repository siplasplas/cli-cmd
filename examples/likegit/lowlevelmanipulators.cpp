#ifdef CLICMD_HEADER_ONLY
#include "cli-cmd.hpp"
#else
#include "cli-cmd.h"
#endif

void addLowLevelManipulators(cli::Application &app)
{
    auto category = app.addCategory("Low-level Commands / Manipulators");
    category->addCommand("apply").desc("Apply a patch to files and/or to the index");
    category->addCommand("checkout-index").desc("Copy files from the index to the working tree");
    category->addCommand("commit-graph").desc("Write and verify Git commit-graph files");
    category->addCommand("commit-tree").desc("Create a new commit object");
    category->addCommand("hash-object").desc("Compute object ID and optionally create an object from a file");
    category->addCommand("index-pack").desc("Build pack index file for an existing packed archive");
    category->addCommand("merge-file").desc("Run a three-way file merge");
    category->addCommand("merge-index").desc("Run a merge for files needing merging");
    category->addCommand("mktag").desc("Creates a tag object with extra validation");
    category->addCommand("mktree").desc("Build a tree-object from ls-tree formatted text");
    category->addCommand("multi-pack-index").desc("Write and verify multi-pack-indexes");
    category->addCommand("pack-objects").desc("Create a packed archive of objects");
    category->addCommand("prune-packed").desc("Remove extra objects that are already in pack files");
    category->addCommand("read-tree").desc("Reads tree information into the index");
    category->addCommand("symbolic-ref").desc("Read, modify and delete symbolic refs");
    category->addCommand("unpack-objects").desc("Unpack objects from a packed archive");
    category->addCommand("update-index").desc("Register file contents in the working tree to the index");
    category->addCommand("update-ref").desc("Update the object name stored in a ref safely");
    category->addCommand("write-tree").desc("Create a tree object from the current index");
}
