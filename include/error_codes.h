#pragma once

namespace cli::ErrorCode {

    inline constexpr int MissingHandler      = 1;
    inline constexpr int TooFewArguments     = 2;
    inline constexpr int TooManyArguments    = 3;
    inline constexpr int UnknownCommand      = 4;
    inline constexpr int BadTokenForm        = 5;
    inline constexpr int UnknownShortOption  = 6;
    inline constexpr int UnknownLongOption   = 7;
    inline constexpr int RequiredParameterMissing = 8;
    inline constexpr int UnexpectedCommandLineEnd       = 9;

} // namespace cli::ErrorCode

namespace cli::ErrorMessage {

    inline constexpr const char* MissingHandler =
        "%s : %s is placeholder with positional arguments:";

    inline constexpr const char* TooFewArguments =
        "%s : %s have %d arguments but minimal is %d";

    inline constexpr const char* TooManyArguments =
        "%s : %s have %d arguments but maximal is %d";

    inline constexpr const char* UnknownCommand =
        "%s: '%s' is not a %s command. See '%s --help'";

    inline constexpr const char* UnknownShortOption =
        "error: unknown short option `%s'";

    inline constexpr const char* UnknownLongOption =
        "error: unknown long option `%s'";

    inline constexpr const char* RequiredParameterMissing =
        "error: required parameter missing `%s'";

    inline constexpr const char* UnexpectedCommandLineEnd =
        "error: unexpected end o command line parameter `%s'";

} // namespace cli::ErrorMessage

namespace cli {
    const char* to_string_errorCode(int code);
}
