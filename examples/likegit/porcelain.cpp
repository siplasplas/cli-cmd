#ifdef CLICMD_HEADER_ONLY
#include "cli-cmd.hpp"
#else
#include "cli-cmd.h"
#endif

void addPorcelainCommands(cli::Application &app)
{
    app.addCategory("Main Porcelain Commands");
}