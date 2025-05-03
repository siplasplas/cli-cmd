#ifdef CLICMD_HEADER_ONLY
#include "cli-cmd.hpp"
#else
#include "cli-cmd.h"
#endif

void addInterrogators(cli::Application &app)
{
    app.addCategory("Ancillary Commands / Interrogators");
}
