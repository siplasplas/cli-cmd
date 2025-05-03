#pragma once
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

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
        std::vector<std::string> categories;
        std::vector<std::string> most_similar_commands(std::string command, const std::map<std::string, std::unique_ptr<Command>> &commands) const;
    protected:
        void help();
    public:
        explicit Application(std::string  app_name);
        void parse(const std::vector<std::string>& args) const;
        void addSubcomand(std::string str, std::function<void()> func);
        void addCategory(std::string caption);
    };

}
