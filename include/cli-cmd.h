#pragma once
#include <functional>
#include <limits>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace cli
{
    class Application;
    class Command;

    using Action = std::function<void(Application*, Command*)>;

    class Flag
    {
        const std::string name, desc;
    public:
        Flag(std::string name,  std::string desc):
            name(std::move(name)), desc(std::move(desc)) {}
        [[nodiscard]] std::string to_string() const;
    };

    struct Argument
    {
        std::string name;
        std::string type;
        Argument(std::string name, std::string type): name(std::move(name)), type(std::move(type)) {}
    };

    struct ArgumentValue
    {
        Argument argument;
        std::string value;
        ArgumentValue(Argument argument, std::string value):
            argument(std::move(argument)), value(std::move(value)) {}
    };

    struct VaArguments
    {
        Argument argument;
        size_t min_n;
        size_t max_n;
        VaArguments(Argument argument, size_t min_n, size_t max_n):
                    argument(std::move(argument)), min_n(min_n), max_n(max_n){}
    };

    class Command {
        const std::string name, desc;
        Action m_handler;
        void parse(int start, const std::vector<std::string>& args);
        friend class Application;
        std::map<std::string, std::shared_ptr<Flag>> availableFlagMap;
        std::vector<std::string> ignoredFlags;
        std::vector<Argument> formalArgList;
        std::optional<VaArguments> formalVaArgs;
    public:
        Command(Action handler, std::string name,  std::string desc):
            name(std::move(name)), desc(std::move(desc)), m_handler(std::move(handler)) {}
        void setHandler(const Action& handler);
        bool containsFlag(const std::string &opt);
        Application* app = nullptr;
        std::vector<ArgumentValue> arguments;
        std::set<std::string> flagSet;
        [[nodiscard]] std::string to_string() const;
        Command& handler(const Action& _handler);
        Command& addArg(std::string name, std::string type);
        Command& addArgs(std::string name, std::string type, size_t min_n, size_t max_n);
        Command& addArgs(std::string name, std::string type, size_t min_n);
        void addFlag(const std::string& str, const std::string& desc);
        void execute();
        void print() const;
    };

    class Subcategory
    {
        const std::string description;
        std::vector<std::shared_ptr<Command>> commands;
        Application* app;
        friend class Application;
    public:
        Subcategory(std::string  name, Application* app): description(std::move(name)), app(app) {}
        [[nodiscard]] std::string to_string() const;
        Command* addSubcomand(const Action& func, std::string str, const std::string& desc);
    };

    class Category
    {
        std::string description;
        std::vector<std::shared_ptr<Command>> commands;
        std::vector<std::unique_ptr<Subcategory>> subcategories;
        friend class Application;
        Application* app;
    public:
        Category(std::string  name, Application* app): description(std::move(name)), app(app) {}
        Category(const Category&) = delete;
        Category& operator=(const Category&) = delete;
        Category(Category&&) = default;
        Category& operator=(Category&&) = default;
        std::string to_string();
        Subcategory* addSubcategory(std::string caption);
        Command* addSubcomand(const Action& func, std::string str, const std::string& desc);
    };

    class Application {
        std::map<std::string, Command*> commandMap;
        std::vector<std::shared_ptr<Command>> commands;
        std::vector<std::unique_ptr<Category>> categories;
        static std::vector<std::string> findMostSimilar(const std::string& proposed, const std::vector<std::string> &keys);
        static std::vector<std::string> splitStringWithQuotes(const std::string& input);
        friend class Category;
        friend class Subcategory;
    public:
        /**
        * @var cmdDepth
        * @brief command hierarchy depth
        *
        * Supported modes:
        * - 0 GCC-style: one command with many options
        * - 1 Flat command list (default): most common simple case for start
        * - 2 Categories: case with many subcommands
        * - 3 Subcategories: even more commands, help shows not all, but subcommands
        *     from subcategories which keep only common subcommands, and
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
         * @example : enable automatic help
         * ```cpp
         * MyApp():cli::Application("myapp", "helpAtStart=1"){}
         * ```
         *
         * @example : default behavior
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
    private:
        static std::unordered_map<std::string, std::string> parseSimpleArgs(const std::string& input);
        static void setArg(std::unordered_map<std::string, std::string> &args,
                           const std::string& name, int& arg, int min, int max);
        void commandNotFound(const std::string &arg);
        void printCommands(Command* cmdHelp) const;
        void commandHelp(Command* cmdHelp) const;
        void proposeSimilar(const std::string &arg) const;
    protected:
        void help(Command* command) const;
        void mainCommandStub(Command*);
        void initSystemCommands();
    public:
        Application(std::string appName, const std::string& namedParams);
        explicit Application(std::string app_name)
            : Application(std::move(app_name), "") {}
        std::string appName;
        std::shared_ptr<Command> mainCommand;
        Command* getCommand(const std::string& name);
        void parse(const std::vector<std::string>& args);
        void parse(const std::string& line);
        void run(int argc, char** argv);
        Command* addSubcomand(const Action& func, const std::string& str, const std::string& desc);
        Category* addCategory(const std::string& caption);
        Command& addCommand(std::string name, const std::string& desc);
    };

}
