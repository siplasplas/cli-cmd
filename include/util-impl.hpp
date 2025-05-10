#pragma once
#include <cstdarg>
#include <stdexcept>
#include "util.h"
#include "iostream"

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

    inline bool isAsciiAlpha(char ch) {
        return std::isalpha(static_cast<unsigned char>(ch));
    }

    INLINE bool isAsciiAlnum(char ch) {
        return std::isalnum(static_cast<unsigned char>(ch));
    }

    INLINE int classifyToken(const std::string& s) {
        if (s.empty()) return ArgError::InvalidEmpty;
        if (s == "-") return ArgError::InvalidDashOnly;

        if (s[0] != '-') {
            // BareIdentifier or Freeform
            if (!isAsciiAlpha(s[0]) || s.back() == '-') return ArgType::Freeform;
            for (char c : s) {
                if (!(isAsciiAlpha(c) || c == '-')) return ArgType::Freeform;
            }
            return ArgType::BareIdentifier;
        }

        // Long option
        if (s.rfind("--", 0) == 0) {
            auto eq = s.find('=');
            std::string key = eq == std::string::npos ? s.substr(2) : s.substr(2, eq - 2);

            if (key.empty() || key.front() == '-' || key.back() == '-')
                return ArgError::InvalidLongOptionSyntax;
            for (char c : key) {
                if (!(isAsciiAlnum(c) || c == '-'))
                    return ArgError::InvalidLongOptionSyntax;
            }
            return (eq != std::string::npos) ? ArgType::LongEquals : ArgType::LongOption;
        }

        // Short option forms
        if (s[0] == '-' && s.size() == 2 && isAsciiAlnum(s[1])) {
            return ArgType::ShortOption;
        }

        if (s[0] == '-' && s.size() > 2) {
            auto eq = s.find('=');
            std::string group = eq == std::string::npos ? s.substr(1) : s.substr(1, eq - 1);

            for (char c : group) {
                if (!isAsciiAlnum(c)) return ArgError::InvalidCompactSyntax;
            }
            if (group.size() == 1)
                return ArgType::ShortEquals;
            else
                return (eq != std::string::npos) ? ArgType::CompactEquals : ArgType::CompactFlags;
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

        case ArgError::InvalidEmpty:            return "InvalidEmpty";
        case ArgError::InvalidDashOnly:         return "InvalidDashOnly";
        case ArgError::InvalidLongOptionSyntax: return "InvalidLongOptionSyntax";

        default: return "<unknown>";
        }
    }

    INLINE std::ostream& operator<<(std::ostream& os, ArgType type) {
        return os << to_string_argtype(static_cast<int>(type));
    }

}
