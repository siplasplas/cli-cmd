#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <nlohmann/json.hpp>
#include "cli-cmd.hpp"

using json = nlohmann::json;

// Dummy handler
void clone_(const cli::Actual*) {}

std::unique_ptr<cli::Application> makeAppWithFlags() {
    auto app = std::make_unique<cli::Application>("test");

    app->addCommand("clone")
        .desc("Clone a repository into a new directory")
        .addArg("repository", "url")
        .addArgs("directory", "path", 0, 1)
        .addFlag("--local", "","")
        .addFlag("--verbose", "-v","")
        .handler(clone_);

    return app;
}

TEST(FlagTest, FlagsFormalDescription) {
    auto app = makeAppWithFlags();

    json expectedFormal = R"({
        "arguments": [
            { "name": "repository", "type": "url" }
        ],
        "flags": [
            { "name": "--local" },
            { "name": "--verbose" }
        ],
        "varargs": {
            "min-n": 0,
            "max-n": 1,
            "name": "directory",
            "type": "path"
        }
    })"_json;

    json formal = app->getCommand("clone")->formalAsJson();
    EXPECT_EQ(expectedFormal, formal);
}

TEST(FlagTest, FlagsParsingAndIgnored) {
    auto app = makeAppWithFlags();

    app->parse("test clone url --local -v --other");

    json expectedActual = R"({
        "command": "clone",
        "error_number": 6,
        "flag_set": ["--local", "--verbose"],
        "arguments": [
            {
                "name": "repository",
                "type": "url",
                "value": "url"
            }
        ]
    })"_json;

    json actual = app->getCommand("clone")->asJson();
    EXPECT_EQ(expectedActual, actual);
}
