#ifdef CLICMD_HEADER_ONLY
#include "cli-cmd.hpp"
#else
#include "cli-cmd.h"
#endif

void addExternalCommands(cli::Application &app)
{
    auto category = app.addCategory("External commands");
    category->addSubcomand(nullptr, "latexdiff","");
}
