#ifdef CLICMD_HEADER_ONLY
#include "cli-cmd.hpp"
#else
#include "cli-cmd.h"
#endif

void addLovLevelInternal(cli::Application &app)
{
    auto category = app.addCategory("Low-level Commands / Internal Helpers");
    category->addCommand("check-attr").desc("Display gitattributes information");
    category->addCommand("check-ignore").desc("Debug gitignore / exclude files");
    category->addCommand("check-mailmap").desc("Show canonical names and email addresses of contacts");
    category->addCommand("check-ref-format").desc("Ensures that a reference name is well formed");
    category->addCommand("column").desc("Display data in columns");
    category->addCommand("credential").desc("Retrieve and store user credentials");
    category->addCommand("credential-cache").desc("Helper to temporarily store passwords in memory");
    category->addCommand("credential-store").desc("Helper to store credentials on disk");
    category->addCommand("fmt-merge-msg").desc("Produce a merge commit message");
    category->addCommand("hook").desc("Run git hooks");
    category->addCommand("interpret-trailers").desc("Add or parse structured information in commit messages");
    category->addCommand("mailinfo").desc("Extracts patch and authorship from a single e-mail message");
    category->addCommand("mailsplit").desc("Simple UNIX mbox splitter program");
    category->addCommand("merge-one-file").desc("The standard helper program to use with git-merge-index");
    category->addCommand("patch-id").desc("Compute unique ID for a patch");
    category->addCommand("sh-i18n").desc("Git's i18n setup code for shell scripts");
    category->addCommand("sh-setup").desc("Common Git shell script setup code");
    category->addCommand("stripspace").desc("Remove unnecessary whitespace");
}
