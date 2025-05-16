#include <gtest/gtest.h>
#include "cli-cmd.hpp"

using namespace cli;

static void dummy_handler(const Actual*) {}

TEST(ParameterTest, OptionalParameterWithValue) {
    Application app("test", 1, 1, 1);
    app.addCommand("build")
        .addParameter("--output", "-o", "Output file path", "general-path")
        .handler(dummy_handler);

    json expectedFormal = R"({
	"arguments": [],
	"flags": [],
	"parameters": [
		{
			"defValue": "",
			"desc": "Output file path",
			"expectType": "general-path",
			"name": "--output",
			"parameterMode": "Optional"
		}
	]
})"_json;
    auto command = app.getCommand("build");
    json formal = command->formalAsJson();
    EXPECT_EQ(expectedFormal, formal);
    app.parse("test build --output /path/to/file");
    auto j = app.currentCommand->asJson();
    nlohmann::json paramMap = j["parameter_map"];
    json expectedParamMap = R"({"--output": "/path/to/file"})"_json;
    EXPECT_EQ(expectedParamMap, paramMap);
}

TEST(ParameterTest, RequiredParameterMissing) {
    Application app("test", 1, 1, 1);
    app.addCommand("build")
        .addReqParameter("--config", "-c", "Configuration file", "general-path")
        .handler(dummy_handler);

    app.parse("test build");
    auto& a1 = app.currentCommand;
    EXPECT_EQ(cli::ErrorCode::RequiredParameterMissing, a1->errNumber);
}

TEST(ParameterTest, DefaultParameterUsed) {
    Application app("test", 1, 1, 1);
    app.addCommand("build")
        .addDefParameter("--log", "-l", "Log output file", "general-path", "default.log")
        .handler(dummy_handler);

    app.parse("test build");

    auto j = app.currentCommand->asJson();
    ASSERT_EQ( "default.log", j["parameter_map"]["--log"]);
}
