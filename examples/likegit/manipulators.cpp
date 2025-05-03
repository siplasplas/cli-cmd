#ifdef CLICMD_HEADER_ONLY
#include "cli-cmd.hpp"
#else
#include "cli-cmd.h"
#endif

void addManipulators(cli::Application &app)
{
    app.addCategory("Ancillary Commands / Manipulators");
}