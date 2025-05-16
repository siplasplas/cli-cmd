#include <gtest/gtest.h>
#include "cli-cmd.hpp"

using json = nlohmann::json;

TEST(EqualsParamTest, AcceptsEqualsSyntax) {
    cli::Application app("test",1, 1, 1);
    app.addCommand("build")
        .addParameter("--output", "-o", "", "path")
        .addParameter("--config", "-c", "", "path")
        .addParameter("--log-file", "", "", "path")
        .handler([](const cli::Actual* a) {
            EXPECT_EQ(a->getValue("--output"), std::optional<std::string>("file.txt"));
            EXPECT_EQ(a->getValue("--config"), std::optional<std::string>("config.json"));
        });

    app.parse("test build --output=file.txt -c=config.json");
    auto cmd = app.currentCommand;
    EXPECT_EQ(0, cmd->errNumber);
    app.execute();
}

TEST(CompactFlagsTest, ExpandsCompactFlags) {
    cli::Application app("test",1, 1, 1);
    app.addCommand("run")
        .addFlag("--verbose", "-v", "")
        .addFlag("--force", "-f", "")
        .addFlag("--debug", "-d", "")
        .addFlag("--dry-run", "-n", "")
        .handler([](const cli::Actual* a) {
            EXPECT_TRUE(a->containsFlag("-v"));
            EXPECT_TRUE(a->containsFlag("-f"));
            EXPECT_TRUE(a->containsFlag("-d"));
        });

    app.parse("test run -vfd");
    auto cmd = app.currentCommand;
    EXPECT_EQ(0, cmd->errNumber);
    app.execute();
}

TEST(CompactFlagsTest, GCCOptions) {
    cli::Application app("test",1, 0, 1);
    app.addCommand("run")
        .addFlag("--verbose", "-v", "")
        .addFlag("--force", "-f", "")
        .addFlag("--debug", "-d", "")
        .addFlag("--dry-run", "-n", "")
        .addParameter("-vfd", "", "", "path")
        .handler([](const cli::Actual* a) {
            EXPECT_FALSE(a->containsFlag("-v"));
            EXPECT_FALSE(a->containsFlag("-f"));
            EXPECT_FALSE(a->containsFlag("-d"));
            EXPECT_EQ(a->getValue("-vfd"), std::optional<std::string>("path/to"));
        });

    app.parse("test run -vfd path/to");
    auto cmd = app.currentCommand;
    EXPECT_EQ(0, cmd->errNumber);
    app.execute();
}
