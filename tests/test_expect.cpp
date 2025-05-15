#include <gtest/gtest.h>
#include "cli-cmd.hpp"

// Dummy handler
static void clone_(const cli::Actual*) {}

TEST(RegisteredTypeTest, BadTypename) {
    cli::Application app("test");
        EXPECT_THROW({
            app.addCommand("clone")
                .desc("Clone a repository into a new directory")
                .addArg("repository", "bad_url") // <== non existed type
                .addArgs("directory", "auto-path", 0, 1)
                .handler([](const cli::Actual*) {});
        }, std::invalid_argument);
}

TEST(TypeTest, ArgType) {
    cli::Application app("test");
    app.addCommand("clone").desc("Clone a repository into a new directory")
            .addArg("repository", "url").addArgs("directory", "auto-path", 0, 1)
            .handler(clone_);
    app.parse("test clone bad_url");
    auto cmd = app.currentCommand;
    EXPECT_EQ(cli::ErrorCode::IsNotExpectedType, cmd->errNumber);

    app.parse("test clone https://github.com/siplasplas/cli-cmd.git");
    cmd = app.currentCommand;
    EXPECT_EQ(0, cmd->errNumber);
}

TEST(TypeTest, ArgsType) {
    cli::Application app("test");
    app.addCommand("clone").desc("Clone a repository into a new directory")
            .addArg("repository", "url").addArgs("directory", "windows-path", 0, 1)
            .handler(clone_);
    app.parse("test clone https://github.com/siplasplas/cli-cmd.git c:\\myrepos\\cli-cmd");
    auto cmd = app.currentCommand;
    EXPECT_EQ(0, cmd->errNumber);

    app.parse("test clone https://github.com/siplasplas/cli-cmd.git c:myrepos\\cli-cmd");
    cmd = app.currentCommand;
    EXPECT_EQ(cli::ErrorCode::IsNotExpectedType, cmd->errNumber);

    app.parse("test clone https://github.com/siplasplas/cli-cmd.git c:\\\\myrepos\\cli-cmd");
    cmd = app.currentCommand;
    EXPECT_EQ(cli::ErrorCode::IsNotExpectedType, cmd->errNumber);
}
