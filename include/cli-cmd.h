#pragma once
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
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
        void execute();
        void print();
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
        std::vector<std::unique_ptr<Command>> commands;
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
        Command* addSubcomand(std::function<void(cli::Application*, Command* command)> func, std::string str,
                              std::string desc);
    };

    class Application {
        std::string appName;
        std::map<std::string, Command*> commandsMap;
        std::vector<std::unique_ptr<Command>> commands;
        std::vector<std::unique_ptr<Category>> categories;
        std::vector<std::string> most_similar_commands(std::string command, const std::map<std::string, Command*> &commands) const;
        std::vector<std::string> splitStringWithQuotes(const std::string& input);
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
        int cmdDepth = 1;
        /**
         * @var combineOpts
         * @brief Allows combining multiple single-letter options after one hyphen
         *
         * When enabled (1), lets users merge options like in UNIX tools:
         * ```bash
         * rm -rf  # Instead of rm -r -f
         * tar -xzf # Instead of tar -x -z -f
         * ```
         *
         * Values:
         * - 0 (default): Each option requires separate hyphen (`-a -b`)
         * - 1: Multiple options can be combined (`-ab`)
         *
         * @note Applies only to:
         * - Single-letter options
         * - Single hyphen prefix
         * - Doesn't affect long options (--help)
         *
         * @warning Combined options cannot take attached values:
         * - ❌ `-j4` (use `-j 4`)
         * - ✅ `-abc -j 4`
         */
        int combineOpts = 0;
        /**
         * @var helpAtStart
         * @brief Controls automatic help display when no commands/options are provided
         *
         * Possible values:
         * - 0 (default): No automatic help - requires explicit `--help` flag
         * - 1: Shows help immediately if no valid commands/options are given
         *
         * @note When enabled (1), these scenarios trigger help:
         * - Empty invocation: `myapp` → shows help
         * - Unknown command: `myapp invalid-cmd` → shows help
         *
         * @example Enable automatic help
         * ```cpp
         * MyApp():cli::Application("myapp", "helpAtStart=1"){}
         * ```
         *
         * @example Default behavior
         * ```cpp
         * MyApp():cli::Application("myapp){}
         * note helpAtStart=0
         * ```
         */
        int helpAtStart = 0;
        /**
         * @var diagnostic
         * @brief Enables diagnostic mode for command tree inspection
         *
         * Security levels:
         * - 0 (default): Production mode - diagnostic commands disabled
         * - 1: Debug mode - enables special diagnostic commands
         *
         * @warning Always set to 0 in production builds!
         *
         * @details When enabled (1), exposes diagnostic subcommands:
         */
        int diagnostic = 0;
        std::unordered_map<std::string, std::string> parseSimpleArgs(const std::string& input);
        void setArg(std::unordered_map<std::string, std::string> &args,
            std::string name, int& arg, int min, int max);
    protected:
        void help(Application*, Command* command);
    public:
        Application(std::string appName, std::string namedParams);
        explicit Application(std::string app_name)
            : Application(std::move(app_name), "") {}
        void parse(const std::vector<std::string>& args);
        void parse(std::string line);
        void run(int argc, char** argv);
        Command* addSubcomand(std::function<void(Application*, Command* command)> func, std::string str, const std::string desc);
        Category* addCategory(std::string caption);
    };

}
