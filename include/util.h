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

    [[nodiscard]] std::string format_message(const char* fmt, ...);
}
