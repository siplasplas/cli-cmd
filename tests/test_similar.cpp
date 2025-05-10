#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "cli-cmd.hpp"

using json = nlohmann::json;

std::unique_ptr<cli::Application> makeAppWithHelpAndClone() {
    auto app = std::make_unique<cli::Application>("test");
    app->addCommand("clone");
    return app;
}

TEST(CliCmdTest, SuggestSimilarCommandBasic) {
    auto app = makeAppWithHelpAndClone();

    // Typo in default command
    app->parse("test hrlp");
    auto& a1 = app->currentCommand;
    EXPECT_EQ(a1->errNumber, cli::ErrorCode::UnknownCommand);
    EXPECT_THAT(a1->mostSimilar, ::testing::ElementsAre("help"));

    // Typo in known command
    app->parse("test cloen");
    auto& a2 = app->currentCommand;
    EXPECT_EQ(a2->errNumber, cli::ErrorCode::UnknownCommand);
    EXPECT_THAT(a2->mostSimilar, ::testing::ElementsAre("clone"));

    // Completely unknown
    app->parse("test computer");
    auto& a3 = app->currentCommand;
    EXPECT_EQ(a3->errNumber, cli::ErrorCode::UnknownCommand);
    EXPECT_TRUE(a3->mostSimilar.empty());
}

std::unique_ptr<cli::Application> makeAppWithCloneAndClean() {
    auto app = std::make_unique<cli::Application>("test");
    app->addCommand("clone").
    app->addCommand("clean");
    return app;
}

TEST(CliCmdTest, SuggestMultipleSimilarCommands) {
    auto app = makeAppWithCloneAndClean();

    app->parse("test clear");
    auto& a1 = app->currentCommand;
    EXPECT_EQ(a1->errNumber, cli::ErrorCode::UnknownCommand);
    EXPECT_THAT(a1->mostSimilar, ::testing::ElementsAre("clean"));

    app->parse("test cle");
    auto& a2 = app->currentCommand;
    EXPECT_EQ(a2->errNumber, cli::ErrorCode::UnknownCommand);
    EXPECT_THAT(a2->mostSimilar, ::testing::UnorderedElementsAre("clean", "clone"));
}
