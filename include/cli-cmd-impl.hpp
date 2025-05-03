#pragma once
#include "cli-cmd.h"
#include "distance-impl.hpp"
namespace cli
{
    INLINE void Command::setPositionalArgsLimits(int min, int max) {
        positional_args = {min, max};
    }

    INLINE std::string Command::to_string()
    {
        return "   "  + name + std::string(std::max(1, 10 - (int)name.size()), ' ') + desc;
    }

    INLINE Command* Subcategory::addSubcomand(std::function<void(cli::Application*, Command* command)> func, std::string str, const std::string desc)
    {
        if (app->commandsMap.find(str) == app->commandsMap.end()) {
            std::unique_ptr<Command> command = std::make_unique<Command>(func, str, desc, app);
            commands.push_back(std::move(command));
            app->commandsMap.emplace(str, commands.back().get());
            return commands.back().get();
        } else throw std::runtime_error("command already exist: " + str);
    }

    INLINE std::string Subcategory::to_string()
    {
        std::string result = name;
        for (const auto& command_ptr : commands)
        {
            result += "\n" + command_ptr->to_string();
        }
        result += "\n";
        return result;
    }

    INLINE std::string Category::to_string()
    {
        return name;
    }

    INLINE Subcategory* Category::addSubcategory(std::string caption)
    {
        auto subcategory = std::make_unique<Subcategory>(std::move(caption), app);
        subcategories.push_back(std::move(subcategory));
        return subcategories.back().get();
    }

    INLINE Application::Application(std::string  app_name) : app_name(std::move(app_name))
    {
    }

    INLINE Category* Application::addCategory(std::string caption)
    {
        auto category = std::make_unique<Category>(caption, this);
        categories.push_back(std::move(category));
        return categories.back().get();
    }

    INLINE void Application::parse(const std::vector<std::string>& args) const
    {
        if (args.size() != 2) return;
        auto it = commandsMap.find(args[1]);
        if (it == commandsMap.end())
        {
            std::cout << app_name << ": '" << args[1] << "' is not a valid command see " <<
                app_name << " --help" << std::endl ;
            auto similars = most_similar_commands(args[1], commandsMap);
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
            Command* command = it->second;
            command->execute();
        }
    }

    INLINE Command* Application::addSubcomand(std::function<void(Application*, Command* command)> func, std::string str, const std::string desc)
    {
        if (commandsMap.find(str) == commandsMap.end()) {
            std::unique_ptr<Command> command = std::make_unique<Command>(func, str, desc, this);
            commands.push_back(std::move(command));
            commandsMap.emplace(str, commands.back().get());
            return commands.back().get();
        } else throw std::runtime_error("command already exist: " + str);
    }

    INLINE void Application::help(Application*, Command* command) {
        for (const auto& category_ptr : categories) {
            auto &subcategories = category_ptr->subcategories;
            for (const auto& subcategory_ptr : subcategories)
            {
                std::cout << subcategory_ptr->to_string() << std::endl;
            }
        }
    }

    INLINE std::vector<std::string> Application::most_similar_commands(std::string command, const std::map<std::string, Command*> &commands) const
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