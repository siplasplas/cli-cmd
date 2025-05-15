#pragma once
#include <string>
#include <vector>

namespace cli
{
    struct Node {
        std::string label;
        std::vector<Node> subnodes;
        explicit Node(std::string label) : label(std::move(label)) {}

        Node* add(Node node) {
            subnodes.push_back(std::move(node));
            return &subnodes.back();
        }
    };

    void printTree(const Node& node, const std::string& prefix = "", bool isLastSibling = true);

    [[nodiscard]] std::string fmt(const char* format_str, ...);

    enum ArgType {
        BareIdentifier = 1,          // e.q. clone
        ShortOption     = 2,         // e.q. -o
        LongOption      = 3,         // e.q. --output
        CompactFlags    = 4,         // e.q. -abc
        Freeform        = 5,         // e.q. ./path/file.txt
        ShortEquals     = 6,         // e.q. -o=/path
        LongEquals      = 7,         // e.q. --output=/path
        CompactEquals   = 8,         // e.q. -abc=out
        GccOption       = 9,         // e.q -abc-def
        GccEquals       = 10,        // e.q -abc-def=/path
    };

    enum ArgError {
        InvalidEmpty             = -1,
        InvalidDashOnly          = -2,
        InvalidLongOptionSyntax  = -3,
        InvalidCompactSyntax     = -4,
        InvalidGccOptionSyntax   = -5,
    };

    int classifyToken(const std::string& s, bool combineOpts);
    const char* to_string_argtype(int code);
    std::ostream& operator<<(std::ostream& os, ArgType type);
    std::string tokenError(const std::string& token, bool combineOpts);
    std::string tokenError(const std::string& token, int expectedType, bool combineOpts);
    std::string tokenError(const std::string& token, const std::vector<int>& expectedTypes, bool combineOpts);
    const char* to_string_errorCode(int code);
}
