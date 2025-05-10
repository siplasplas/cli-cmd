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
