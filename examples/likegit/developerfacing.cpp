#ifdef CLICMD_HEADER_ONLY
#include "cli-cmd.hpp"
#else
#include "cli-cmd.h"
#endif

void addDeveloperFacing(cli::Application &app)
{
    auto category = app.addCategory("Developer-facing file formats, protocols and other interfaces");
    category->addCommand("format-bundle").desc("The bundle file format");
    category->addCommand("format-chunk").desc("Chunk-based file formats");
    category->addCommand("format-commit-graph").desc("Git commit-graph format");
    category->addCommand("format-index").desc("Git index format");
    category->addCommand("format-pack").desc("Git pack format");
    category->addCommand("format-signature").desc("Git cryptographic signature formats");
    category->addCommand("protocol-capabilities").desc("Protocol v0 and v1 capabilities");
    category->addCommand("protocol-common").desc("Things common to various protocols");
    category->addCommand("protocol-http").desc("Git HTTP-based protocols");
    category->addCommand("protocol-pack").desc("How packs are transferred over-the-wire");
    category->addCommand("protocol-v2").desc("Git Wire Protocol, Version 2");
}

