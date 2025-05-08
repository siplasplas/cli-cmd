#ifdef CLICMD_HEADER_ONLY
#include "cli-cmd.hpp"
#else
#include "cli-cmd.h"
#endif

void addDeveloperFacing(cli::Application &app)
{
    auto category = app.addCategory("Developer-facing file formats, protocols and other interfaces");
    category->addCommand("format-bundle", "The bundle file format");
    category->addCommand("format-chunk", "Chunk-based file formats");
    category->addCommand("format-commit-graph", "Git commit-graph format");
    category->addCommand("format-index", "Git index format");
    category->addCommand("format-pack", "Git pack format");
    category->addCommand("format-signature", "Git cryptographic signature formats");
    category->addCommand("protocol-capabilities", "Protocol v0 and v1 capabilities");
    category->addCommand("protocol-common", "Things common to various protocols");
    category->addCommand("protocol-http", "Git HTTP-based protocols");
    category->addCommand("protocol-pack", "How packs are transferred over-the-wire");
    category->addCommand("protocol-v2", "Git Wire Protocol, Version 2");
}

