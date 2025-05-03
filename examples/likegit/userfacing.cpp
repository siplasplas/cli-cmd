#ifdef CLICMD_HEADER_ONLY
#include "cli-cmd.hpp"
#else
#include "cli-cmd.h"
#endif

void adduserFacing(cli::Application &app)
{
    app.addCategory("User-facing repository, command and file interfaces");
}