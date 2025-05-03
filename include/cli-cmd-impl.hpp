#pragma once
#include "distance.h"
#include "distance-impl.hpp"
namespace cli
{
    INLINE Application::Application(std::string  app_name) : app_name(std::move(app_name))
    {
        addSubcomand("help", std::bind(&Application::help, this));
    }


    INLINE void Application::addCategory(std::string caption)
    {
        categories.push_back(caption);
    }

    INLINE void Application::parse(const std::vector<std::string>& args) const
    {
        if (args.size() != 2 || commands.find(args[1]) == commands.end())
        {
            std::cout << app_name << ": '" << args[1] << "' is not a valid command see " <<
                app_name << " --help" << std::endl ;
            auto similars = most_similar_commands(args[1], commands);
            if (similars.size() > 0) {
                if (similars.size() > 1)
                    std::cout << "The most similar commands are" << std::endl;
                else
                    std::cout << "The most similar command is" << std::endl;
                for (const auto& similar : similars)
                    std::cout << "     " << similar << std::endl;
            }
        } else {
            std::string name = args[1];
            Command* command = commands.at(name).get();
            command->execute();
        }
    }

    INLINE void Application::addSubcomand(const std::string str, std::function<void()> func)
    {
        if (commands.find(str) == commands.end()) {
            std::unique_ptr<Command> command = std::make_unique<Command>(func);
            commands[str] = std::move(command);
        } else throw std::runtime_error("command already exist: " + str);
    }

    INLINE void Application::help() {
        for (auto category : categories)
            std::cout << category << std::endl;
    }

    INLINE std::vector<std::string> Application::most_similar_commands(std::string command, const std::map<std::string, std::unique_ptr<Command>> &commands) const
    {
        const int maxDist = 5;
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