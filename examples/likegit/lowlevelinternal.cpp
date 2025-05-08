#ifdef CLICMD_HEADER_ONLY
#include "cli-cmd.hpp"
#else
#include "cli-cmd.h"
#endif

void addLovLevelInternal(cli::Application &app)
{
    auto category = app.addCategory("Low-level Commands / Internal Helpers");
    category->addCommand("check-attr", "Display gitattributes information");
    category->addCommand("check-ignore", "Debug gitignore / exclude files");
    category->addCommand("check-mailmap", "Show canonical names and email addresses of contacts");
    category->addCommand("check-ref-format", "Ensures that a reference name is well formed");
    category->addCommand("column", "Display data in columns");
    category->addCommand("credential", "Retrieve and store user credentials");
    category->addCommand("credential-cache", "Helper to temporarily store passwords in memory");
    category->addCommand("credential-store", "Helper to store credentials on disk");
    category->addCommand("fmt-merge-msg", "Produce a merge commit message");
    category->addCommand("hook", "Run git hooks");
    category->addCommand("interpret-trailers", "Add or parse structured information in commit messages");
    category->addCommand("mailinfo", "Extracts patch and authorship from a single e-mail message");
    category->addCommand("mailsplit", "Simple UNIX mbox splitter program");
    category->addCommand("merge-one-file", "The standard helper program to use with git-merge-index");
    category->addCommand("patch-id", "Compute unique ID for a patch");
    category->addCommand("sh-i18n", "Git's i18n setup code for shell scripts");
    category->addCommand("sh-setup", "Common Git shell script setup code");
    category->addCommand("stripspace", "Remove unnecessary whitespace");
}
