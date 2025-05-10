#pragma once
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include <nlohmann/json.hpp>
using nlohmann::json;

namespace cli
{
    struct Actual;
    class Application;
    class Command;
    struct ArgumentValue;

    void to_json(json& j, const ArgumentValue& v);
    void to_json(json& j, const Actual& a);

    using Action = std::function<void(Actual*)>;

    class Flag
    {
    public:
        Flag(std::string name,  std::string desc):
            name(std::move(name)), desc(std::move(desc)) {}
        const std::string name, desc;
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

    struct Actual
    {
        virtual ~Actual() = default;
        std::string m_name;
        std::vector<std::string> ignoredFlags;
        std::vector<ArgumentValue> arguments;
        std::set<std::string> flagSet;
        [[nodiscard]] bool containsFlag(const std::string &opt) const;
        int errNumber = 0;
        std::optional<std::string> errorStr;
        std::vector<std::string> mostSimilar;
        explicit Actual(std::string commandName): m_name(std::move(commandName)){}
    };

    struct Formal
    {
        std::map<std::string, std::shared_ptr<Flag>> availableFlagMap;
        std::vector<Argument> argList;
        std::optional<VaArguments> vaArgs;
    };

    class Command: public Actual {
        std::string m_desc;
        Action m_handler;
        void parse(int start, const std::vector<std::string>& args);
        friend class Application;
    public:
        Command(std::string name, Application* app): Actual(std::move(name)), app(app) {}
        Formal formal;
        Application* app;
        void commandNotFound(const std::string &arg);
        json asJson();
        json formalAsJson();
        [[nodiscard]] std::string to_string() const;
        Command& handler(const Action& _handler);
        Command& desc(const std::string& _desc);
        Command& addArg(std::string name, std::string type);
        Command& addArgs(std::string name, std::string type, size_t min_n, size_t max_n);
        Command& addArgs(std::string name, std::string type, size_t min_n);
        Command& addFlag(const std::string& str, const std::string& desc);
        void execute();
        void print() const;
    };

    class Category
    {
        std::string description;
        std::vector<std::shared_ptr<Command>> commands;
        friend class Application;
        Application* app;
    public:
        Category(std::string  description, Application* app): description(std::move(description)), app(app) {}
        Category(const Category&) = delete;
        Category& operator=(const Category&) = delete;
        Category& ref(const std::string& commandName);
        Category(Category&&) = default;
        Category& operator=(Category&&) = default;
        std::string to_string();
        static  bool isAlphaNumOrDash(const std::string& str);
        static void checkCommandName(const std::string& commandName);
        Command& addCommand(std::string commandName);
    };

    class Application {
        std::map<std::string, std::shared_ptr<Command>> commandMap;
        std::vector<std::shared_ptr<Command>> commands;
        std::vector<std::unique_ptr<Category>> categories;
        std::vector<std::unique_ptr<Category>> helpCategories;
        static std::vector<std::string> findMostSimilar(const std::string& proposed, const std::vector<std::string> &keys);
        static std::vector<std::string> splitStringWithQuotes(const std::string& input);
        friend class Command;
        friend class Category;
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
        void printCommands(const Actual* actual) const;
        void commandHelp(Actual* actual);
        [[nodiscard]] std::vector<std::string>  proposeSimilar(const std::string &arg) const;
    protected:
        void help(Actual*);
        void mainCommandStub(Actual*);
        void initSystemCommands();
    public:
        Application(std::string appName, const std::string& namedParams);
        explicit Application(std::string app_name)
            : Application(std::move(app_name), "") {}
        std::string appName;
        std::shared_ptr<Command> mainCommand;
        std::shared_ptr<Command> currentCommand;
        void execute();
        std::shared_ptr<Command> getCommand(const std::string& name);
        void parse(const std::vector<std::string>& args);
        void parse(const std::string& line);
        void parse(int argc, char** argv);
        void run(int argc, char** argv);
        Category* addCategory(const std::string& caption);
        Category& addHelpCategory(const std::string& caption);
        Command& addCommand(std::string name);
    };

}
