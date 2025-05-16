#include <gtest/gtest.h>
#include "cli-cmd.hpp"

// Dummy handler
static int dummy_handler(const cli::Actual*) {return 1;}

TEST(RegisteredTypeTest, BadTypename) {
    cli::Application app("test", 1, 1, 1);
        EXPECT_THROW({
            app.addCommand("clone")
                .desc("Clone a repository into a new directory")
                .addArg("repository", "bad_url") // <== non existed type
                .addArgs("directory", "auto-path", 0, 1)
                .handler([](const cli::Actual*) {return 0;});
        }, std::invalid_argument);
}

TEST(TypeTest, ArgType) {
    cli::Application app("test", 1, 1, 1);
    app.addCommand("clone").desc("Clone a repository into a new directory")
            .addArg("repository", "url").addArgs("directory", "auto-path", 0, 1)
            .handler(dummy_handler);
    app.parse("test clone bad_url");
    auto cmd = app.currentCommand;
    EXPECT_EQ(cli::ErrorCode::IsNotExpectedTypeArg, cmd->errNumber);

    app.parse("test clone https://github.com/siplasplas/cli-cmd.git");
    cmd = app.currentCommand;
    EXPECT_EQ(0, cmd->errNumber);
}

TEST(TypeTest, ArgsType) {
    cli::Application app("test", 1, 1, 1);
    app.addCommand("clone").desc("Clone a repository into a new directory")
            .addArg("repository", "url").addArgs("directory", "windows-path", 0, 1)
            .handler(dummy_handler);
    app.parse("test clone https://github.com/siplasplas/cli-cmd.git c:\\myrepos\\cli-cmd");
    auto cmd = app.currentCommand;
    EXPECT_EQ(0, cmd->errNumber);

    app.parse("test clone https://github.com/siplasplas/cli-cmd.git c:myrepos\\cli-cmd");
    cmd = app.currentCommand;
    EXPECT_EQ(cli::ErrorCode::IsNotExpectedTypeArg, cmd->errNumber);

    app.parse("test clone https://github.com/siplasplas/cli-cmd.git c:\\\\myrepos\\cli-cmd");
    cmd = app.currentCommand;
    EXPECT_EQ(cli::ErrorCode::IsNotExpectedTypeArg, cmd->errNumber);
}

TEST(TypeTest, ParameterType) {
    cli::Application app("test", 1, 1, 1);
    app.addCommand("build")
        .addParameter("--output", "-o", "Output file path", "windows-path")
        .handler(dummy_handler);
    app.parse("test build -o c:\\path\\to\\file");
    auto cmd = app.currentCommand;
    EXPECT_EQ(0, cmd->errNumber);

    app.parse("test build -o c:path\\to\\file");
    cmd = app.currentCommand;
    EXPECT_EQ(cli::ErrorCode::IsNotExpectedTypeParam, cmd->errNumber);
}