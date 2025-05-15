#include <gtest/gtest.h>
#include "cli-cmd.hpp"

using namespace cli;

void dummy_handler(const Actual*) {}

TEST(ParameterTest, OptionalParameterWithValue) {
    Application app("test");
    app.addCommand("build")
        .addParameter("--output", "-o", "linux-path", "Output file path")
        .handler(dummy_handler);

    json expectedFormal = R"({
	"arguments": [],
	"flags": [],
	"parameters": [
		{
			"defValue": "",
			"desc": "Output file path",
			"expectType": "linux-path",
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
    Application app("test");
    app.addCommand("build")
        .addReqParameter("--config", "-c", "linux-path", "Configuration file")
        .handler(dummy_handler);

    app.parse("test build");
    auto& a1 = app.currentCommand;
    EXPECT_EQ(cli::ErrorCode::RequiredParameterMissing, a1->errNumber);
}

TEST(ParameterTest, DefaultParameterUsed) {
    Application app("test");
    app.addCommand("build")
        .addDefParameter("--log", "-l", "default.log", "linux-path", "Log output file")
        .handler(dummy_handler);

    app.parse("test build");

    auto j = app.currentCommand->asJson();
    ASSERT_EQ( "default.log", j["parameter_map"]["--log"]);
}
