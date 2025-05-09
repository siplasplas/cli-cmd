#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include "cli-cmd.hpp"

using json = nlohmann::json;

void expectJsonEqual(const json& expected, const json& actual) {
    if (expected != actual) {
        std::cerr << "\n=== JSON mismatch ===\n";
        std::cerr << "Expected:\n" << expected.dump(2) << "\n";
        std::cerr << "Actual:\n"   << actual.dump(2)   << "\n";
    }
    EXPECT_EQ(expected, actual);
}

std::unique_ptr<cli::Application> makeAppWithClone() {
    auto app = std::make_unique<cli::Application>("test");

    app->addCommand("clone")
        .desc("Clone a repository into a new directory")
        .addArg("repository", "url")
        .addArgs("directory", "path", 0, 1)
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
        "varargs": {
            "argument": {
                "name": "directory",
                "type": "path"
            },
            "min-n": 0,
            "max-n": 1
        }
    })"_json;

    json formal = app->getCommand("clone")->formalAsJson();
    expectJsonEqual(expectedFormal, formal);
}

TEST(CliCmdTest, ArgumentsActualParsing) {
    auto app = makeAppWithClone();

    app->parse("myapp clone path -v -a -b -a");

    json expectedActual = R"({
        "command": "clone",
        "error_number": 0,
        "flag_set": [],
        "ignored_flags": ["-v", "-a", "-b", "-a"],
        "arguments": [
            {
                "name": "repository",
                "type": "url",
                "value": "path"
            }
        ]
    })"_json;

    json actual = app->getCommand("clone")->asJson();
    expectJsonEqual(expectedActual, actual);
}
