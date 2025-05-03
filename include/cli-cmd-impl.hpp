#pragma once
#include "cli-cmd.h"
#include "distance-impl.hpp"
namespace cli
{
    INLINE Command* Subcategory::addSubcomand(std::function<void()> func, std::string str, const std::string desc)
    {
        if (commands.find(str) == commands.end()) {
            std::unique_ptr<Command> command = std::make_unique<Command>(func, str, desc);
            auto raw_ptr = command.get();
            commands[str] = std::move(command);
            return raw_ptr;
        } else throw std::runtime_error("command already exist: " + str);
    }

    INLINE std::string Category::to_string()
    {
        return name;
    }

    INLINE Subcategory* Category::addSubcategory(std::string caption)
    {
        auto subcategory = std::make_unique<Subcategory>(std::move(caption));
        Subcategory* raw_ptr = subcategory.get();
        subcategories.push_back(std::move(subcategory));
        return raw_ptr;
    }

    INLINE Application::Application(std::string  app_name) : app_name(std::move(app_name))
    {
        addSubcomand(std::bind(&Application::help, this), "help", "Display help information about likegit");
    }


    INLINE Category* Application::addCategory(std::string caption)
    {
        auto category = std::make_unique<Category>(caption);
        Category* raw_ptr = category.get();
        categories.push_back(std::move(category));
        return raw_ptr;
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

    INLINE Command* Application::addSubcomand(std::function<void()> func, std::string str, const std::string desc)
    {
        if (commands.find(str) == commands.end()) {
            std::unique_ptr<Command> command = std::make_unique<Command>(func, str, desc);
            auto raw_ptr = command.get();
            commands[str] = std::move(command);
            return raw_ptr;
        } else throw std::runtime_error("command already exist: " + str);
    }

    INLINE void Application::help() {
        for (const auto& category_ptr : categories) {
            std::cout << category_ptr->to_string() << std::endl;
        }
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