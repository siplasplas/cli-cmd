#ifdef CLICMD_HEADER_ONLY
#include "cli-cmd.hpp"
#else
#include "cli-cmd.h"
#endif

void addDeveloperFacing(cli::Application &app)
{
    auto category = app.addCategory("Developer-facing file formats, protocols and other interfaces");
    category->addSubcomand(nullptr, "format-bundle", "The bundle file format");
    category->addSubcomand(nullptr, "format-chunk", "Chunk-based file formats");
    category->addSubcomand(nullptr, "format-commit-graph", "Git commit-graph format");
    category->addSubcomand(nullptr, "format-index", "Git index format");
    category->addSubcomand(nullptr, "format-pack", "Git pack format");
    category->addSubcomand(nullptr, "format-signature", "Git cryptographic signature formats");
    category->addSubcomand(nullptr, "protocol-capabilities", "Protocol v0 and v1 capabilities");
    category->addSubcomand(nullptr, "protocol-common", "Things common to various protocols");
    category->addSubcomand(nullptr, "protocol-http", "Git HTTP-based protocols");
    category->addSubcomand(nullptr, "protocol-pack", "How packs are transferred over-the-wire");
    category->addSubcomand(nullptr, "protocol-v2", "Git Wire Protocol, Version 2");
}

