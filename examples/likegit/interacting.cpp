#ifdef CLICMD_HEADER_ONLY
#include "cli-cmd.hpp"
#else
#include "cli-cmd.h"
#endif

void addInteractingCommands(cli::Application &app)
{
    app.addCategory("Interacting with Others");
}