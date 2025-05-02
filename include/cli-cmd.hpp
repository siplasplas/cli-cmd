#pragma once
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "similar.hpp"

namespace cli
{
    class Command {
    public:
        explicit Command(std::function<void()> handler) : handler(std::move(handler)){}
        const std::function<void()> handler;
        void execute() const
        {
            handler();
        }
    };

    class Application {
        std::string app_name;
        std::map<std::string, std::unique_ptr<Command>> commands;

    public:
        explicit Application(std::string  app_name);
        void parse(const std::vector<std::string>& args) const;
        void addSubcomand(std::string str, std::function<void()> func);
    };

    Application::Application(std::string  app_name) : app_name(std::move(app_name))
    {
    }

    void Application::parse(const std::vector<std::string>& args) const
    {
        if (args.size() != 2 || commands.find(args[1]) == commands.end())
        {
            std::cout << app_name << ": '" << args[1] << "' is not a valid command see " <<
                app_name << " --help" << std::endl ;
            auto similars = most_similar_commands<std::unique_ptr<Command>>(args[1], commands);
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

    void Application::addSubcomand(const std::string str, std::function<void()> func)
    {
        if (commands.find(str) == commands.end()) {
            std::unique_ptr<Command> command = std::make_unique<Command>(func);
            commands[str] = std::move(command);
        } else throw std::runtime_error("command already exist: " + str);
    }
}