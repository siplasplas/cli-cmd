#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "cli-cmd.hpp"

using json = nlohmann::json;

TEST(GlobaLocalTest, SumFlags) {
    cli::Application app("test", 1, 1, 1);
    app.addFlag("--all","-a","")
    .addFlag("--bbb","-b","")
    .addFlag("--ccc","-c","")
    .addCommand("samplecmd")
    .addFlag("--ddd","-d","")
    .addFlag("--eee","-e","");
    app.parse("test samplecmd");
    auto cmd = app.getCommand("samplecmd");
    EXPECT_EQ(5, cmd->availableOptionMap.size());
    app.execute();
}

TEST(GlobaLocalTest, SumParameters) {
    cli::Application app("test", 1, 1, 1);
    app.addParameter("--aaa", "-a", "", "string")
    .addReqParameter("--bbb", "-b", "", "string")
    .addDefParameter("--ccc", "-c", "", "string", "default")
    .addCommand("samplecmd")
    .addParameter("--ddd", "-d", "", "string")
    .addReqParameter("--eee", "-e", "", "string")
    .addDefParameter("--fff", "-f", "", "string", "default");

    app.parse("test samplecmd");
    auto cmd = app.getCommand("samplecmd");
    EXPECT_EQ(6, cmd->availableOptionMap.size());
    app.execute();
}