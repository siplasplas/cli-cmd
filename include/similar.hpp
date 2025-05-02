#pragma once
#include "distance.hpp"

namespace cli
{
    const int maxDist = 5;
    template <typename T>
    std::vector<std::string> most_similar_commands(std::string command, const std::map<std::string, T> &commands)
    {
        std::vector<std::string> result;
        int bestLen = maxDist;
        for (const auto& other : commands)
        {
            cli ::Distance dist(command, other.first);
            int d = dist.compare();
            if (d <= bestLen)
            {
                if (d < bestLen)
                {
                    result.clear();
                    bestLen = d;
                }
                result.push_back(other.first);
            }
        }
        return result;
    }
}