#ifdef CLICMD_HEADER_ONLY
#include "cli-cmd.hpp"
#else
#include "cli-cmd.h"
#endif

void addLowLevelSyncing(cli::Application &app)
{
    auto category = app.addCategory("Low-level Commands / Syncing Repositories");
    category->addCommand("daemon").desc("A really simple server for Git repositories");
    category->addCommand("fetch-pack").desc("Receive missing objects from another repository");
    category->addCommand("http-backend").desc("Server side implementation of Git over HTTP");
    category->addCommand("send-pack").desc("Push objects over Git protocol to another repository");
    category->addCommand("update-server-info").desc("Update auxiliary info file to help dumb servers");
}


