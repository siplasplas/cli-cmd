#pragma once
#include <cstdarg>
#include <stdexcept>
#include "util.h"

namespace cli
{
    /**
     * @brief Formats a message using printf-style syntax (temporary implementation)
     *
     * @param fmt Format string (printf-style)
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
     * @example format_message("Error %d: %s", 42, "File not found");
     *
     * @throws std::runtime_error if formatting fails
     */
    INLINE std::string format_message(const char* fmt, ...) {
        // First call - get required length
        va_list args;
        va_start(args, fmt);
        const int length = vsnprintf(nullptr, 0, fmt, args);
        va_end(args);

        if (length < 0) {
            throw std::runtime_error("Formatting error");
        }

        // Allocate buffer with exact size needed
        std::vector<char> buf(length + 1); // +1 for null terminator

        // Second call - actual formatting
        va_start(args, fmt);
        vsnprintf(buf.data(), buf.size(), fmt, args);
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

}
