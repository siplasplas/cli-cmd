#include <gtest/gtest.h>
#include "cli-cmd.hpp"

using namespace cli;

TEST(ClassifyTokenTest, BareIdentifiers) {
    EXPECT_EQ(classifyToken("clone", 1), ArgType::BareIdentifier);
    EXPECT_EQ(classifyToken("init-repo", 1), ArgType::BareIdentifier);
    EXPECT_EQ(classifyToken("status-check", 1), ArgType::BareIdentifier);
}

TEST(ClassifyTokenTest, ShortOptions) {
    EXPECT_EQ(classifyToken("-v", 1), ArgType::ShortOption);
    EXPECT_EQ(classifyToken("-o", 1), ArgType::ShortOption);
    EXPECT_EQ(classifyToken("-o=/path", 1), ArgType::ShortEquals);
}

TEST(ClassifyTokenTest, LongOptions) {
    EXPECT_EQ(classifyToken("--help", 1), ArgType::LongOption);
    EXPECT_EQ(classifyToken("--output", 1), ArgType::LongOption);
    EXPECT_EQ(classifyToken("--output=/path/to/file", 1), ArgType::LongEquals);
    EXPECT_EQ(classifyToken("--dry-run", 1), ArgType::LongOption);
}

TEST(ClassifyTokenTest, CompactFlags) {
    EXPECT_EQ(classifyToken("-abc", 1), ArgType::CompactFlags);
    EXPECT_EQ(classifyToken("-vo=out.txt", 1), ArgType::CompactEquals);
}

TEST(ClassifyTokenTest, FreeformValues) {
    EXPECT_EQ(classifyToken("./file.txt", 1), ArgType::Freeform);
    EXPECT_EQ(classifyToken("https://repo.com", 1), ArgType::Freeform);
    EXPECT_EQ(classifyToken("path/with-dash", 1), ArgType::Freeform);
}

TEST(ClassifyTokenTest, InvalidTokens) {
    EXPECT_EQ(classifyToken("", 1), ArgError::InvalidEmpty);
    EXPECT_EQ(classifyToken("-", 1), ArgError::InvalidDashOnly);
    EXPECT_EQ(classifyToken("--", 1), ArgError::InvalidLongOptionSyntax);
    EXPECT_EQ(classifyToken("--a", 1), ArgError::InvalidLongOptionSyntax);
    EXPECT_EQ(classifyToken("---", 1), ArgError::InvalidLongOptionSyntax);
    EXPECT_EQ(classifyToken("---a", 1), ArgError::InvalidLongOptionSyntax);
    EXPECT_EQ(classifyToken("---abc", 1), ArgError::InvalidLongOptionSyntax);
    EXPECT_EQ(classifyToken("--bad-", 1), ArgError::InvalidLongOptionSyntax);
    EXPECT_EQ(classifyToken("--@invalid", 1), ArgError::InvalidLongOptionSyntax);
}

TEST(ClassifyTokenTest, CombineOpts) {
    EXPECT_EQ(classifyToken("-abc", 1), ArgType::CompactFlags);
    EXPECT_EQ(classifyToken("-abc=/path", 1), ArgType::CompactEquals);
    EXPECT_EQ(classifyToken("-abc-def", 1), ArgError::InvalidCompactSyntax);
    EXPECT_EQ(classifyToken("-abc-def=/path", 1), ArgError::InvalidCompactSyntax);
    EXPECT_EQ(classifyToken("-a", 0), ArgType::ShortOption);
    EXPECT_EQ(classifyToken("-a=/path", 0), ArgType::ShortEquals);
    EXPECT_EQ(classifyToken("-abc", 0), ArgType::GccOption);
    EXPECT_EQ(classifyToken("-abc=/path", 0), ArgType::GccEquals);
    EXPECT_EQ(classifyToken("-abc-def", 0), ArgType::GccOption);
    EXPECT_EQ(classifyToken("-abc-def=/path", 0), ArgType::GccEquals);
}


TEST(ClassifyTokenTest, Digits) {
    EXPECT_EQ(classifyToken("sh-i18n", 1), ArgType::BareIdentifier);
    EXPECT_EQ(classifyToken("sh-i18", 1), ArgType::BareIdentifier);
    EXPECT_EQ(classifyToken("123sh-i18", 1), ArgType::Freeform);
    EXPECT_EQ(classifyToken("-2", 1), ArgError::InvalidCompactSyntax);
    EXPECT_EQ(classifyToken("-2=/path", 1), ArgError::InvalidCompactSyntax);
    EXPECT_EQ(classifyToken("--1abc123-d1", 1), ArgError::InvalidLongOptionSyntax);
    EXPECT_EQ(classifyToken("--abc123-d1", 1), ArgType::LongOption);
    EXPECT_EQ(classifyToken("--abc123-d1-", 1), ArgError::InvalidLongOptionSyntax);
    EXPECT_EQ(classifyToken("--abc123-d1=/path/to/file", 1), ArgType::LongEquals);
    EXPECT_EQ(classifyToken("-ab2c", 1), ArgError::InvalidCompactSyntax);
    EXPECT_EQ(classifyToken("-ab2c=out.txt", 1), ArgError::InvalidCompactSyntax);
    EXPECT_EQ(classifyToken("-1ab2c", 0), ArgError::InvalidGccOptionSyntax);
    EXPECT_EQ(classifyToken("-ab2c", 0), ArgType::GccOption);
    EXPECT_EQ(classifyToken("-ab2c-", 0), ArgError::InvalidGccOptionSyntax);
    EXPECT_EQ(classifyToken("-ab2c=out.txt", 0), ArgType::GccEquals);
}

TEST(TokenErrorTest, TokenMustMatchSingleType) {
    EXPECT_EQ(tokenError("--config", ArgType::LongOption, 1), "");
    EXPECT_FALSE(tokenError("--config", ArgType::ShortOption, 1).empty());
    EXPECT_FALSE(tokenError("-v", ArgType::LongOption, 1).empty());
}

TEST(TokenErrorTest, TokenMustMatchAnyOfMultipleTypes) {
    EXPECT_EQ(tokenError("-v", {ArgType::ShortOption, ArgType::CompactFlags}, 1), "");
    EXPECT_EQ(tokenError("-abc", {ArgType::CompactFlags}, 1), "");
    EXPECT_FALSE(tokenError("--bad-option-", {ArgType::ShortOption, ArgType::CompactFlags}, 1).empty());
}
