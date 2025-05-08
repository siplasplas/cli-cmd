#ifdef CLICMD_HEADER_ONLY
#include "cli-cmd.hpp"
#else
#include "cli-cmd.h"
#endif

void addLowLevelManipulators(cli::Application &app)
{
    auto category = app.addCategory("Low-level Commands / Manipulators");
    category->addCommand("apply", "Apply a patch to files and/or to the index");
    category->addCommand("checkout-index", "Copy files from the index to the working tree");
    category->addCommand("commit-graph", "Write and verify Git commit-graph files");
    category->addCommand("commit-tree", "Create a new commit object");
    category->addCommand("hash-object", "Compute object ID and optionally create an object from a file");
    category->addCommand("index-pack", "Build pack index file for an existing packed archive");
    category->addCommand("merge-file", "Run a three-way file merge");
    category->addCommand("merge-index", "Run a merge for files needing merging");
    category->addCommand("mktag", "Creates a tag object with extra validation");
    category->addCommand("mktree", "Build a tree-object from ls-tree formatted text");
    category->addCommand("multi-pack-index", "Write and verify multi-pack-indexes");
    category->addCommand("pack-objects", "Create a packed archive of objects");
    category->addCommand("prune-packed", "Remove extra objects that are already in pack files");
    category->addCommand("read-tree", "Reads tree information into the index");
    category->addCommand("symbolic-ref", "Read, modify and delete symbolic refs");
    category->addCommand("unpack-objects", "Unpack objects from a packed archive");
    category->addCommand("update-index", "Register file contents in the working tree to the index");
    category->addCommand("update-ref", "Update the object name stored in a ref safely");
    category->addCommand("write-tree", "Create a tree object from the current index");
}
