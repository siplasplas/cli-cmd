#pragma once
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "distance.h"

namespace cli
{
    class Command {
        const std::string name, desc;
    public:
        Command(std::function<void()> handler, std::string name,  std::string desc):
            name(name), desc(desc),handler(std::move(handler)) {}
        const std::function<void()> handler;
        void execute() const
        {
            handler();
        }
    };

    class Subcategory
    {
        const std::string name;
        std::map<std::string, std::unique_ptr<Command>> commands;
    public:
        explicit Subcategory(const std::string name): name(name) {}
        std::string to_string();
        Command* addSubcomand(std::function<void()> func, std::string str, const std::string desc);
    };

    class Category
    {
        std::string name;
        std::vector<std::unique_ptr<Subcategory>> subcategories;
    public:
        explicit Category(const std::string name): name(name) {}
        Category(const Category&) = delete;
        Category& operator=(const Category&) = delete;
        Category(Category&&) = default;
        Category& operator=(Category&&) = default;
        std::string to_string();
        Subcategory* addSubcategory(std::string caption);
    };

    class Application {
        std::string app_name;
        std::map<std::string, std::unique_ptr<Command>> commands;
        std::vector<std::unique_ptr<Category>> categories;
        std::vector<std::string> most_similar_commands(std::string command, const std::map<std::string, std::unique_ptr<Command>> &commands) const;
    protected:
        void help();
    public:
        explicit Application(std::string  app_name);
        void parse(const std::vector<std::string>& args) const;
        Command* addSubcomand(std::function<void()> func, std::string str, const std::string desc);
        Category* addCategory(std::string caption);
    };

}
