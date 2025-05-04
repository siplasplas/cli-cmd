#ifdef CLICMD_HEADER_ONLY
#include "cli-cmd.hpp"
#else
#include "cli-cmd.h"
#endif

void addLovLevelInternal(cli::Application &app)
{
    auto category = app.addCategory("Low-level Commands / Internal Helpers");
    category->addSubcomand(nullptr, "check-attr", "Display gitattributes information");
    category->addSubcomand(nullptr, "check-ignore", "Debug gitignore / exclude files");
    category->addSubcomand(nullptr, "check-mailmap", "Show canonical names and email addresses of contacts");
    category->addSubcomand(nullptr, "check-ref-format", "Ensures that a reference name is well formed");
    category->addSubcomand(nullptr, "column", "Display data in columns");
    category->addSubcomand(nullptr, "credential", "Retrieve and store user credentials");
    category->addSubcomand(nullptr, "credential-cache", "Helper to temporarily store passwords in memory");
    category->addSubcomand(nullptr, "credential-store", "Helper to store credentials on disk");
    category->addSubcomand(nullptr, "fmt-merge-msg", "Produce a merge commit message");
    category->addSubcomand(nullptr, "hook", "Run git hooks");
    category->addSubcomand(nullptr, "interpret-trailers", "Add or parse structured information in commit messages");
    category->addSubcomand(nullptr, "mailinfo", "Extracts patch and authorship from a single e-mail message");
    category->addSubcomand(nullptr, "mailsplit", "Simple UNIX mbox splitter program");
    category->addSubcomand(nullptr, "merge-one-file", "The standard helper program to use with git-merge-index");
    category->addSubcomand(nullptr, "patch-id", "Compute unique ID for a patch");
    category->addSubcomand(nullptr, "sh-i18n", "Git's i18n setup code for shell scripts");
    category->addSubcomand(nullptr, "sh-setup", "Common Git shell script setup code");
    category->addSubcomand(nullptr, "stripspace", "Remove unnecessary whitespace");
}
