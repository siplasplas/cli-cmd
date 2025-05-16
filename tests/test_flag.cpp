#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <nlohmann/json.hpp>
#include "cli-cmd.hpp"

using json = nlohmann::json;

// Dummy handler
static void clone_(const cli::Actual*) {}

std::unique_ptr<cli::Application> makeAppWithFlags() {
    auto app = std::make_unique<cli::Application>("test", 1, 1, 1);

    app->addCommand("clone")
        .desc("Clone a repository into a new directory")
        .addArg("repository", "url")
        .addArgs("directory", "auto-path", 0, 1)
        .addFlag("--local", "","")
        .addFlag("--verbose", "-v","")
        .addFlag("-B", "-B","")
        .addFlag("-S", "","")
        .handler(clone_);

    return app;
}

TEST(FlagTest, FlagsFormalDescription) {
    auto app = makeAppWithFlags();

    json expectedFormal = R"({
        "arguments": [
            { "name": "repository", "expectType": "url" }
        ],
        "flags": [
            {
                "flagMode": "Present",
                "name": "--local"
            },
            {
                "flagMode": "Present",
                "name": "--verbose"
            }
            ,
            {
                "flagMode": "Present",
                "name": "-B"
            }
            ,
            {
                "flagMode": "Present",
                "name": "-S"
            }
        ],
        "parameters": [],
        "varargs": {
            "min-n": 0,
            "max-n": 1,
            "name": "directory",
            "expectType": "auto-path"
        }
    })"_json;

    json formal = app->getCommand("clone")->formalAsJson();
    EXPECT_EQ(expectedFormal, formal);
}

TEST(FlagTest, FlagsParsingAndIgnored) {
    auto app = makeAppWithFlags();

    app->parse("test clone https://github.com/a/b.git --local -B -S -v --other");

    json expectedActual = R"({
        "command": "clone",
        "error": "UnknownLongOption",
        "flag_set": ["--local", "--verbose", "-B", "-S"],
        "parameter_map": {},
        "arguments": [
            {
                "name": "repository",
                "expectType": "url",
                "value": "https://github.com/a/b.git"
            }
        ]
    })"_json;

    json actual = app->getCommand("clone")->asJson();
    EXPECT_EQ(expectedActual, actual);
}
