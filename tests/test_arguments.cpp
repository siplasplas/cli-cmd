#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include "cli-cmd.hpp"

using json = nlohmann::json;

std::unique_ptr<cli::Application> makeAppWithClone() {
    auto app = std::make_unique<cli::Application>("test");

    app->addCommand("clone")
        .desc("Clone a repository into a new directory")
        .addArg("repository", "url")
        .addArgs("directory", "auto-path", 0, 1)
        .handler([](const cli::Actual*) {}); // test stub

    return app;
}

TEST(CliCmdTest, ArgumentsFormalDescription) {
    auto app = makeAppWithClone();

    json expectedFormal = R"({
        "arguments": [
            { "name": "repository", "type": "url" }
        ],
        "flags": [],
        "parameters": [],
        "varargs": {
            "min-n": 0,
            "max-n": 1,
            "name": "directory",
            "type": "auto-path"
        }
    })"_json;

    json formal = app->getCommand("clone")->formalAsJson();
    EXPECT_EQ(expectedFormal, formal);
}

TEST(CliCmdTest, ArgumentsActualParsing) {
    auto app = makeAppWithClone();

    app->parse("mycli clone https://github.com/siplasplas/cli-cmd.git");

    json expectedActual = R"({
        "command": "clone",
        "flag_set": [],
        "parameter_map": {},
        "arguments": [
            {
                "name": "repository",
                "type": "url",
                "value": "https://github.com/siplasplas/cli-cmd.git"
            }
        ]
    })"_json;

    json actual = app->getCommand("clone")->asJson();
    EXPECT_EQ(expectedActual, actual);
}
