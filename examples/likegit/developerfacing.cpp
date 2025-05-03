#ifdef CLICMD_HEADER_ONLY
#include "cli-cmd.hpp"
#else
#include "cli-cmd.h"
#endif

void addDeveloperFacing(cli::Application &app)
{
    app.addCategory("Developer-facing file formats, protocols and other interfaces");
}