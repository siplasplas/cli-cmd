#ifdef CLICMD_HEADER_ONLY
#include "cli-cmd.hpp"
#else
#include "cli-cmd.h"
#endif

void addLowLevelSyncing(cli::Application &app)
{
    auto category = app.addCategory("Low-level Commands / Syncing Repositories");
    category->addSubcomand(nullptr, "daemon", "A really simple server for Git repositories");
    category->addSubcomand(nullptr, "fetch-pack", "Receive missing objects from another repository");
    category->addSubcomand(nullptr, "http-backend", "Server side implementation of Git over HTTP");
    category->addSubcomand(nullptr, "send-pack", "Push objects over Git protocol to another repository");
    category->addSubcomand(nullptr, "update-server-info", "Update auxiliary info file to help dumb servers");
}


