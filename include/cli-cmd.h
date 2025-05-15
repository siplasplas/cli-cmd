#pragma once
#include "util.h"
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

    enum class OptionKind { Flag, Parameter };

    enum class FlagMode {
        Present,
        Hidden
    };

    enum class ParameterMode {
        Optional,
        Required,
        Defaulted,
        Hidden
    };

    class Option {
    protected:
        std::string m_name;
        std::string m_description;
    public:
        Option(std::string name, std::string description)
            : m_name(std::move(name)), m_description(std::move(description)) {}

        virtual ~Option() = default;

        [[nodiscard]] const std::string& name() const { return m_name; }
        [[nodiscard]] const std::string& description() const { return m_description; }
        [[nodiscard]] virtual std::string to_string() const = 0;
        [[nodiscard]] virtual OptionKind kind() const = 0;
    };


    class Flag: public Option
    {
        FlagMode m_flagMode;
    public:
        Flag(std::string name, std::string description, FlagMode flagMode)
            : Option(std::move(name), std::move(description)),m_flagMode(flagMode) {}

        Flag(const Flag& base, FlagMode overrideMode)
            : Option(base.name(), base.description()), m_flagMode(overrideMode) {}

        [[nodiscard]] std::string to_string() const override;
        [[nodiscard]] OptionKind kind() const override {
            return OptionKind::Flag;
        };
        [[nodiscard]] FlagMode flagMode() const{
            return m_flagMode;
        }
    };

    class Parameter: public Option
    {
        std::string m_expect;
        ParameterMode m_parameterMode;
        std::string m_defValue;
    public:
        Parameter(std::string name, std::string description, std::string defVal, std::string expectType,
            ParameterMode parameterMode)
                : Option(std::move(name), std::move(description)),m_expect(std::move(expectType)),
                m_parameterMode(parameterMode), m_defValue(std::move(defVal)){}

        Parameter(const Parameter& base, ParameterMode overrideMode, std::string defVal = "")
               : Option(base.name(), base.description()),
                m_expect(base.m_expect),m_parameterMode(overrideMode), m_defValue(std::move(defVal)) {}

        [[nodiscard]] std::string to_string() const override;
        [[nodiscard]] OptionKind kind() const override {
            return OptionKind::Parameter;
        };
        [[nodiscard]] ParameterMode parameterMode() const{
            return m_parameterMode;
        }
        [[nodiscard]] std::string expect() const {
            return m_expect;
        }
        [[nodiscard]] std::string defValue() const {
            return m_defValue;
        }
    };

    struct Argument
    {
        std::string name;
        std::string type;
        Argument()= default;
        Argument(std::string name, std::string type): name(std::move(name)), type(std::move(type)) {}
    };

    struct ArgumentValue
    {
        Argument argument;
        std::string value;
        ArgumentValue(Argument argument, std::string value):
            argument(std::move(argument)), value(std::move(value)) {}
    };

    struct VaArguments: public Argument
    {
        size_t min_n = 0;
        size_t max_n = 0;
        VaArguments()= default;
        VaArguments(std::string name, std::string type, size_t min_n, size_t max_n):
            Argument(std::move(name), std::move(type)), min_n(min_n), max_n(max_n){}
    };

    struct Actual
    {
        virtual ~Actual() = default;
        std::string m_name;
        std::vector<ArgumentValue> arguments;
        std::set<std::string> flagSet;
        std::map<std::string, std::string> parameterMap;
        int errNumber = 0;
        std::optional<std::string> errorStr;
        std::vector<std::string> mostSimilar;
        static std::optional<std::string> getParamValue(const std::string &key);
        [[nodiscard]] bool containsFlag(const std::string &opt) const;
        explicit Actual(std::string commandName): m_name(std::move(commandName)){}
        void clearActual();
    };

    struct Formal
    {
        std::map<std::string, std::shared_ptr<Option>> optionMap;
        std::vector<Argument> argList;
        VaArguments vaArgs;
            void addFlag(Application* app, const std::string& name, const std::string& shorthand, const std::string& desc);
        void addParameter(Application * app, const std::string& name, const std::string& shorthand,
            const std::string& defValue, const std::string& expect, ParameterMode parameterMode, const std::string& desc);
    private:
        static void checkNames(Application *app, const std::string &name, const std::string &shorthand);
        static void addShorthand(Application *app, const std::string &name, const std::string &shorthand);
    };

    class Command: public Actual {
        std::string m_desc;
        Action m_handler;
        void parse(int start, const std::vector<std::string>& args);
        friend class Application;
        void printSimilars();
        void printErrors();
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
        Command& addFlag(const std::string& name, const std::string& shorthand, const std::string& desc);
        Command& addParameter(const std::string& name, const std::string& shorthand, const std::string& expect, const std::string& desc);
        Command &addReqParameter(const std::string&, const std::string&, const std::string&, const std::string&);
        Command &addDefParameter(const std::string&, const std::string&, const std::string&, const std::string&,
                                 const std::string&);
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
        Command& addCommand(std::string commandName);
    };

    class Application {
        std::map<std::string, std::shared_ptr<Command>> commandMap;
        std::vector<std::shared_ptr<Command>> commands;
        Formal formal;
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
        int combineOpts = 1;
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
        std::map<std::string, std::string> shorthandMap;
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
        Command& addCommand(std::string commandName);
    };

}
