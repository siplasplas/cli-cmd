#ifdef CLICMD_HEADER_ONLY
#include "cli-cmd.hpp"
#else
#include "cli-cmd.h"
#endif

void addLowLevelSyncing(cli::Application &app)
{
    app.addCategory("Low-level Commands / Syncing Repositories");
}