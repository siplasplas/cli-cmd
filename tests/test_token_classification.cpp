#include <gtest/gtest.h>
#include "cli-cmd.hpp"

using namespace cli;

TEST(ClassifyTokenTest, BareIdentifiers) {
    EXPECT_EQ(classifyToken("clone"), ArgType::BareIdentifier);
    EXPECT_EQ(classifyToken("init-repo"), ArgType::BareIdentifier);
    EXPECT_EQ(classifyToken("status-check"), ArgType::BareIdentifier);
}

TEST(ClassifyTokenTest, ShortOptions) {
    EXPECT_EQ(classifyToken("-v"), ArgType::ShortOption);
    EXPECT_EQ(classifyToken("-o"), ArgType::ShortOption);
    EXPECT_EQ(classifyToken("-o=/path"), ArgType::ShortEquals);
}

TEST(ClassifyTokenTest, LongOptions) {
    EXPECT_EQ(classifyToken("--help"), ArgType::LongOption);
    EXPECT_EQ(classifyToken("--output"), ArgType::LongOption);
    EXPECT_EQ(classifyToken("--output=/path/to/file"), ArgType::LongEquals);
    EXPECT_EQ(classifyToken("--dry-run"), ArgType::LongOption);
}

TEST(ClassifyTokenTest, CompactFlags) {
    EXPECT_EQ(classifyToken("-abc"), ArgType::CompactFlags);
    EXPECT_EQ(classifyToken("-vo=out.txt"), ArgType::CompactEquals);
}

TEST(ClassifyTokenTest, FreeformValues) {
    EXPECT_EQ(classifyToken("./file.txt"), ArgType::Freeform);
    EXPECT_EQ(classifyToken("https://repo.com"), ArgType::Freeform);
    EXPECT_EQ(classifyToken("path/with-dash"), ArgType::Freeform);
}

TEST(ClassifyTokenTest, InvalidTokens) {
    EXPECT_EQ(classifyToken(""), ArgError::InvalidEmpty);
    EXPECT_EQ(classifyToken("-"), ArgError::InvalidDashOnly);
    EXPECT_EQ(classifyToken("--"), ArgError::InvalidLongOptionSyntax);
    EXPECT_EQ(classifyToken("--a"), ArgError::InvalidLongOptionSyntax);
    EXPECT_EQ(classifyToken("---"), ArgError::InvalidLongOptionSyntax);
    EXPECT_EQ(classifyToken("---a"), ArgError::InvalidLongOptionSyntax);
    EXPECT_EQ(classifyToken("---abc"), ArgError::InvalidLongOptionSyntax);
    EXPECT_EQ(classifyToken("--bad-"), ArgError::InvalidLongOptionSyntax);
    EXPECT_EQ(classifyToken("--@invalid"), ArgError::InvalidLongOptionSyntax);
}


TEST(TokenErrorTest, ValidTokenReturnsEmptyString) {
    EXPECT_EQ(tokenError("clone"), "");
    EXPECT_EQ(tokenError("-v"), "");
    EXPECT_EQ(tokenError("--output"), "");
    EXPECT_EQ(tokenError("-abc"), "");
    EXPECT_EQ(tokenError("--log=debug"), "");
    EXPECT_EQ(tokenError("-o=val"), "");
}

TEST(TokenErrorTest, InvalidTokenReturnsErrorString) {
    EXPECT_FALSE(tokenError("").empty());
    EXPECT_FALSE(tokenError("-").empty());
    EXPECT_FALSE(tokenError("---").empty());
    EXPECT_FALSE(tokenError("--a").empty()); // too short
    EXPECT_FALSE(tokenError("--@invalid").empty());
}

TEST(TokenErrorTest, TokenMustMatchSingleType) {
    EXPECT_EQ(tokenError("--config", ArgType::LongOption), "");
    EXPECT_FALSE(tokenError("--config", ArgType::ShortOption).empty());
    EXPECT_FALSE(tokenError("-v", ArgType::LongOption).empty());
}

TEST(TokenErrorTest, TokenMustMatchAnyOfMultipleTypes) {
    EXPECT_EQ(tokenError("-v", {ArgType::ShortOption, ArgType::CompactFlags}), "");
    EXPECT_EQ(tokenError("-abc", {ArgType::CompactFlags}), "");
    EXPECT_FALSE(tokenError("--bad-option-", {ArgType::ShortOption, ArgType::CompactFlags}).empty());
}