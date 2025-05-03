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
    class Application;

    struct PositionalArgsLimits {
        int min;
        int max; /// MAXINT means no limit
    };

    class Command {
        const std::string name, desc;
        PositionalArgsLimits positionalLimit{0, 0};
        std::function<void(Application*, Command* command)> handler;
        Application* app = nullptr;
        void initPositional(int start, const std::vector<std::string>& args);
        friend class Application;
    public:
        Command(std::function<void(Application*, Command* command)> handler, std::string name,  std::string desc):
            name(name), desc(desc), handler(std::move(handler)) {}
        std::vector<std::string> positionalArgs;
        std::string to_string();
        void setPositionalArgsLimits(int min, int max);
        void execute()
        {
            if (!handler)
                std::cout << "Placeholder: command not set" << std::endl;
            else
                handler(app, this);
        }
    };

    class Subcategory
    {
        const std::string name;
        std::vector<std::unique_ptr<Command>> commands;
        Application* app;
        friend class Application;
    public:
        Subcategory(const std::string name, Application* app): name(name), app(app) {}
        std::string to_string();
        Command* addSubcomand(std::function<void(Application*, Command* command)> func, std::string str, const std::string desc);
        void addOption(std::string str, const std::string& desc);
    };

    class Category
    {
        std::string name;
        std::vector<std::unique_ptr<Subcategory>> subcategories;
        friend class Application;
        Application* app;
    public:
        Category(const std::string name, Application* app): name(name), app(app) {}
        Category(const Category&) = delete;
        Category& operator=(const Category&) = delete;
        Category(Category&&) = default;
        Category& operator=(Category&&) = default;
        std::string to_string();
        Subcategory* addSubcategory(std::string caption);
    };

    class Application {
        std::string appName;
        std::map<std::string, Command*> commandsMap;
        std::vector<std::unique_ptr<Command>> commands;
        std::vector<std::unique_ptr<Category>> categories;
        std::vector<std::string> most_similar_commands(std::string command, const std::map<std::string, Command*> &commands) const;
        friend class Category;
        friend class Subcategory;
        /**
        * @var cmdDepth
        * @brief command hierarchy depth
        *
        * Supported modes:
        * - 0 GCC-style: one command with many options
        * - 1 Flat command list (default): most common simple case for start
        * - 2 Categories: case with many subcommands
        * - 3 Subcategories: even more commands, help shows not all, but subcommands
        *     from subcategories whichs keep only common subcommands, and
        *     help --all shows all
        */
        int cmdDepth;
    protected:
        void help(Application*, Command* command);
    public:
        Application(std::string appName, int depth)
            : appName(std::move(appName)), cmdDepth(depth)
        {
            if (!appName.empty())
                throw std::invalid_argument("appName is empty");
            if (depth < 0 || depth > 3)
                throw std::invalid_argument("cmdDepth must be betwen 0 and 3");
        }
        explicit Application(std::string app_name)
            : Application(std::move(app_name), 1) {}
        void parse(const std::vector<std::string>& args);
        void run(int argc, char** argv);
        Command* addSubcomand(std::function<void(Application*, Command* command)> func, std::string str, const std::string desc);
        Category* addCategory(std::string caption);
    };

}
