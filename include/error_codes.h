#pragma once

namespace cli::ErrorCode {

    inline constexpr int MissingHandler      = 1;
    inline constexpr int TooFewArguments     = 2;
    inline constexpr int TooManyArguments    = 3;
    inline constexpr int UnknownCommand      = 4;

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

} // namespace cli::ErrorMessage
