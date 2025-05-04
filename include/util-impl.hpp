#pragma once
namespace cli
{
    INLINE void printTree(const Node& node, const std::string& prefix = "", bool isLastSibling = true)
    {
        std::cout << prefix << (isLastSibling ? "└── " : "├── ") << node.label << "\n";

        std::string newPrefix = prefix + (isLastSibling ? "    " : "│   ");
        for (size_t i = 0; i < node.subnodes.size(); ++i) {
            bool isLastChild = (i == node.subnodes.size() - 1);
            printTree(node.subnodes[i], newPrefix, isLastChild);
        }
    }

}