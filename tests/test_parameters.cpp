#include <gtest/gtest.h>
#include "cli-cmd.hpp"

using namespace cli;

void dummy_handler(const Actual*) {}

TEST(ParameterTest, OptionalParameterWithValue) {
    Application app("test");
    app.addCommand("build")
        .addParameter("--output", "-o", "path", "Output file path")
        .handler(dummy_handler);

    app.parse("test build -o out.txt");

    auto j = app.currentCommand->asJson();
    ASSERT_EQ(j["parameters"]["--output"], "out.txt");
}

TEST(ParameterTest, RequiredParameterMissing) {
    Application app("test");
    app.addCommand("build")
        .addReqParameter("--config", "-c", "path", "Configuration file")
        .handler(dummy_handler);

    app.parse("test build");
    auto& a1 = app.currentCommand;
    EXPECT_EQ(a1->errNumber, cli::ErrorCode::RequiredParameterMissing);
}

TEST(ParameterTest, DefaultParameterUsed) {
    Application app("test");
    app.addCommand("build")
        .addDefParameter("--log", "-l", "path", "default.log", "Log output file")
        .handler(dummy_handler);

    app.parse("test build");

    auto j = app.currentCommand->asJson();
    ASSERT_EQ(j["parameters"]["--log"], "default.log");
}
