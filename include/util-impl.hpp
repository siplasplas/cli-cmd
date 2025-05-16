#pragma once
#include <cstdarg>
#include <stdexcept>
#include "util.h"
#include "iostream"
#include "error_codes.h"

namespace cli
{
    /**
     * @brief Formats a message using printf-style syntax (temporary implementation)
     *
     * @param format_str Format string (printf-style)
     * @param ... Variable arguments matching the format specifiers
     *
     * @return std::string Formatted message
     *
     * @note This is a temporary implementation that will be replaced with std::format
     *       when the codebase transitions to C++20. It's provided to maintain
     *       compatibility with pre-C++20 projects using this library.
     *
     * @warning Avoid using this function for new code - it will be deprecated
     *          once C++20 becomes the minimum required standard.
     *
     * @example fmt("Error %d: %s", 42, "File not found");
     *
     * @throws std::runtime_error if formatting fails
     */
    INLINE std::string fmt(const char* format_str, ...) {
        // First call - get required length
        va_list args;
        va_start(args, format_str);
        const int length = vsnprintf(nullptr, 0, format_str, args);
        va_end(args);

        if (length < 0) {
            throw std::runtime_error("Formatting error");
        }

        // Allocate buffer with exact size needed
        std::vector<char> buf(length + 1); // +1 for null terminator

        // Second call - actual formatting
        va_start(args, format_str);
        vsnprintf(buf.data(), buf.size(), format_str, args);
        va_end(args);

        return std::string(buf.data());
    }

    INLINE void printTree(const Node& node, const std::string& prefix, bool isLastSibling)
    {
        std::cout << prefix << (isLastSibling ? "└── " : "├── ") << node.label << "\n";

        std::string newPrefix = prefix + (isLastSibling ? "    " : "│   ");
        for (size_t i = 0; i < node.subnodes.size(); ++i) {
            bool isLastChild = (i == node.subnodes.size() - 1);
            printTree(node.subnodes[i], newPrefix, isLastChild);
        }
    }

    INLINE int classifyToken(const std::string& s, bool combineOpts)
    {
        if (s.empty()) return ArgError::InvalidEmpty;
        if (s == "-") return ArgError::InvalidDashOnly;

        if (s[0] != '-') {
            // BareIdentifier or Freeform
            if (!isalpha(s[0]) || s.back() == '-') return ArgType::Freeform;
            for (char c : s) {
                if (!(isalnum(c) || c == '-')) return ArgType::Freeform;
            }
            return ArgType::BareIdentifier;
        }

        // Long option
        if (s.rfind("--", 0) == 0) {
            auto eq = s.find('=');
            std::string key = eq == std::string::npos ? s.substr(2) : s.substr(2, eq - 2);

            if (key.size() < 2 || !isalpha(key.front()) || key.back() == '-')
                return ArgError::InvalidLongOptionSyntax;
            for (char c : key) {
                if (!(isalnum(c) || c == '-'))
                    return ArgError::InvalidLongOptionSyntax;
            }
            return (eq != std::string::npos) ? ArgType::LongEquals : ArgType::LongOption;
        }

        if (s[0] == '-' && s.size() > 1) {
            auto eq = s.find('=');
            std::string group = eq == std::string::npos ? s.substr(1) : s.substr(1, eq - 1);

            if (!combineOpts) {
                // GCC style: whole string after '-' interpreted as one option
                // can have '-' inside
                if (!isalpha(group.front()) || group.back() == '-')
                    return ArgError::InvalidGccOptionSyntax;
                for (char c : group) {
                    if (!isalnum(c) && c != '-') return ArgError::InvalidGccOptionSyntax;
                }
                if (group.size() == 1)
                    return (eq != std::string::npos) ? ArgType::ShortEquals : ArgType::ShortOption;
                else
                    return (eq != std::string::npos) ? ArgType::GccEquals : ArgType::GccOption;
            } else {
                // Git-style compact flags (only letters/digits, without '-')
                for (char c : group) {
                    if (!isalpha(c)) return ArgError::InvalidCompactSyntax;
                }
                if (group.size() == 1)
                    return (eq != std::string::npos) ? ArgType::ShortEquals : ArgType::ShortOption;
                else
                    return (eq != std::string::npos) ? ArgType::CompactEquals : ArgType::CompactFlags;
            }
        }

        return ArgType::Freeform;
    }

    INLINE const char* to_string_argtype(int code) {
        using namespace cli;

        switch (code) {
        case ArgType::BareIdentifier: return "BareIdentifier";
        case ArgType::ShortOption:    return "ShortOption";
        case ArgType::LongOption:     return "LongOption";
        case ArgType::CompactFlags:   return "CompactFlags";
        case ArgType::Freeform:       return "Freeform";
        case ArgType::ShortEquals:    return "ShortEquals";
        case ArgType::LongEquals:     return "LongEquals";
        case ArgType::CompactEquals:  return "CompactEquals";
        case ArgType::GccOption:      return "GccOption";
        case ArgType::GccEquals:      return "GccEquals";

        case ArgError::InvalidEmpty:            return "InvalidEmpty";
        case ArgError::InvalidDashOnly:         return "InvalidDashOnly";
        case ArgError::InvalidLongOptionSyntax: return "InvalidLongOptionSyntax";
        case ArgError::InvalidCompactSyntax:    return "InvalidCompactSyntax";
        case ArgError::InvalidGccOptionSyntax:  return "InvalidGccOptionSyntax";

        default: return "<unknown>";
        }
    }

    INLINE std::ostream& operator<<(std::ostream& os, ArgType type) {
        return os << to_string_argtype(static_cast<int>(type));
    }

    INLINE std::string tokenError(const std::string& token, int expectedType, bool combineOpts) {
        int type = classifyToken(token, combineOpts);
        return (type != expectedType)
            ? "Token " + token + " expected " + to_string_argtype(expectedType) + " but got " + to_string_argtype(type)
            : "";
    }

    INLINE std::string tokenError(const std::string& token, const std::vector<int>& expectedTypes, bool combineOpts) {
        int type = classifyToken(token, combineOpts);
        for (int e : expectedTypes) {
            if (type == e) return "";
        }
        std::string list;
        for (size_t i = 0; i < expectedTypes.size(); ++i) {
            if (i > 0) list += ", ";
            list += to_string_argtype(expectedTypes[i]);
        }
        return "Token " + token + " expected one of: [" + list + "], but got " + to_string_argtype(type);
    }

    INLINE const char* to_string_errorCode(int code) {
        using namespace cli;
        switch (code) {
            case ErrorCode::MissingHandler: return "MissingHandler";
            case ErrorCode::TooFewArguments: return "TooFewArguments";
            case ErrorCode::TooManyArguments: return "TooManyArguments";
            case ErrorCode::UnknownCommand: return "UnknownCommand";
            case ErrorCode::BadTokenForm: return "BadTokenForm";
            case ErrorCode::UnknownShortOption: return "UnknownShortOption";
            case ErrorCode::UnknownLongOption: return "UnknownLongOption";
            case ErrorCode::RequiredParameterMissing: return "RequiredParameterMissing";
            case ErrorCode::UnexpectedCommandLineEnd: return "UnexpectedCommandLineEnd";
            case ErrorCode::OptionUsedTwice: return "OptionUsedTwice";
            default: return "<unknown>";
        }
    }

    INLINE std::pair<std::string, std::string> splitEquals(const std::string& token) {
        auto pos = token.find('=');
        if (pos == std::string::npos) {
            return {token, ""};
        }
        return {
            token.substr(0, pos),
            token.substr(pos + 1)
        };
    }

}
