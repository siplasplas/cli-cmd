#ifdef CLICMD_HEADER_ONLY
#include "cli-cmd.hpp"
#else
#include "cli-cmd.h"
#endif

void addLowLevelManipulators(cli::Application &app)
{
    auto category = app.addCategory("Low-level Commands / Manipulators");
    category->addSubcomand(nullptr, "apply", "Apply a patch to files and/or to the index");
    category->addSubcomand(nullptr, "checkout-index", "Copy files from the index to the working tree");
    category->addSubcomand(nullptr, "commit-graph", "Write and verify Git commit-graph files");
    category->addSubcomand(nullptr, "commit-tree", "Create a new commit object");
    category->addSubcomand(nullptr, "hash-object", "Compute object ID and optionally create an object from a file");
    category->addSubcomand(nullptr, "index-pack", "Build pack index file for an existing packed archive");
    category->addSubcomand(nullptr, "merge-file", "Run a three-way file merge");
    category->addSubcomand(nullptr, "merge-index", "Run a merge for files needing merging");
    category->addSubcomand(nullptr, "mktag", "Creates a tag object with extra validation");
    category->addSubcomand(nullptr, "mktree", "Build a tree-object from ls-tree formatted text");
    category->addSubcomand(nullptr, "multi-pack-index", "Write and verify multi-pack-indexes");
    category->addSubcomand(nullptr, "pack-objects", "Create a packed archive of objects");
    category->addSubcomand(nullptr, "prune-packed", "Remove extra objects that are already in pack files");
    category->addSubcomand(nullptr, "read-tree", "Reads tree information into the index");
    category->addSubcomand(nullptr, "symbolic-ref", "Read, modify and delete symbolic refs");
    category->addSubcomand(nullptr, "unpack-objects", "Unpack objects from a packed archive");
    category->addSubcomand(nullptr, "update-index", "Register file contents in the working tree to the index");
    category->addSubcomand(nullptr, "update-ref", "Update the object name stored in a ref safely");
    category->addSubcomand(nullptr, "write-tree", "Create a tree object from the current index");
}
