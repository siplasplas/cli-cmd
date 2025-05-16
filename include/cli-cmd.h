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
        std::string m_expectType;
        ParameterMode m_parameterMode;
        std::string m_defValue;
    public:
        Parameter(std::string name, std::string description, std::string defVal, std::string expectType,
            ParameterMode parameterMode);

        Parameter(const Parameter& base, ParameterMode overrideMode, std::string defVal = "")
               : Option(base.name(), base.description()),
                m_expectType(base.m_expectType),m_parameterMode(overrideMode), m_defValue(std::move(defVal)) {}

        [[nodiscard]] std::string to_string() const override;
        [[nodiscard]] OptionKind kind() const override {
            return OptionKind::Parameter;
        };
        [[nodiscard]] ParameterMode parameterMode() const{
            return m_parameterMode;
        }
        [[nodiscard]] std::string expectType() const {
            return m_expectType;
        }
        [[nodiscard]] std::string defValue() const {
            return m_defValue;
        }
    };

    class Argument
    {
        std::string m_name;
        std::string m_expectType;
    public:
        Argument()= default;
        Argument(std::string name, std::string expectType);
        [[nodiscard]] std::string name() const {
            return m_name;
        }
        [[nodiscard]] std::string expectType() const {
            return m_expectType;
        }
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
        std::map<std::string, std::shared_ptr<Option>> availableOptionMap = {};
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

    class Formal
    {
        static void checkNames(Application *app, const std::string &name, const std::string &shorthand);
        static void addShorthand(Application *app, const std::string &name, const std::string &shorthand);
        bool isGlobal;
    public:
        explicit Formal(bool isGlobal):isGlobal(isGlobal){}
        std::map<std::string, std::shared_ptr<Option>> optionMap = {};
        std::vector<Argument> argList = {};
        VaArguments vaArgs = {};
        void addFlag(Application* app, const std::string& name, const std::string& shorthand, const std::string& desc);
        void addParameter(Application *app, const std::string& name, const std::string& shorthand,
            const std::string& desc, const std::string &expect, const std::string &defValue,
            ParameterMode parameterMode);
    };

    class Command: public Actual {
        std::string m_desc;
        Action m_handler;
        void parse(int start, const std::vector<std::string>& args);
        void parseHelpCommand(int start, const std::vector<std::string>& args);
        friend class Application;
        void printSimilars();
        void printErrors();
        void buildMergedOptions();
        std::set<std::string> hiddenOptNames = {};
    public:
        Command(std::string name, Application* app): Actual(std::move(name)), formal(false),app(app) {}
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
        Command& addFlag(const std::string& name, const std::string& shorthand,
            const std::string& desc);
        Command& addParameter(const std::string& name, const std::string& shorthand,
            const std::string& desc, const std::string &expect);
        Command &addReqParameter(const std::string& name, const std::string& shorthand,
                                 const std::string& desc, const std::string &expect);
        Command &addDefParameter(const std::string& name, const std::string& shorthand,
            const std::string& desc, const std::string &expect, const std::string &defValue);
        Command &overrideParameter(const std::string &name, ParameterMode parameterMode,
            const std::string& defValue = "");
        Command &hideOption(const std::string &name);
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
        friend class Formal;
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
        int cmdDepth;
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
        int combineOpts;
        /**
         * @brief Controls help availability in the CLI application.
         *
         * The `helpAvailability` variable determines in which situations the CLI help system (`--help` flag or `help` command)
         * is enabled or shown to the user.
         *
         * Values:
         * - 0 – help is available only via the `--help` flag
         * - 1 – in addition to `--help`, a dedicated `help` command is available
         * - 2 – help is also shown automatically when the program is run without any commands or options
         *
         * The default value is typically 1.
         */
       int helpAvailability;
    private:
        void printCommands(const Actual* actual) const;
        void commandHelp(Actual* actual);
        [[nodiscard]] std::vector<std::string>  proposeSimilar(const std::string &arg) const;
        static bool findHelpOption(const std::vector<std::string> &args);
        void helpAboutHelp() const;
        /**
         * @brief Locks the definition of global options after commands are created.
         *
         * This flag is used to enforce the ordering rule that all global options
         * (such as global flags or parameters) must be defined before any command is added.
         *
         * Once a command is added via Application::addCommand() or Category::addCommand,
         * this flag is set to true.
         * Subsequent attempts to add global flags or parameters will throw a logic_error.
         *
         * This prevents ambiguity and ensures consistent override behavior between
         * global and command-specific options.
         */
        bool globalOptionsLocked = false;
    protected:
        void help(Actual*);
        void mainCommandStub(Actual*);
        void initSystemCommands();
        void registerValidators();
    public:
        std::map<std::string, std::string> shorthandMap;
        ~Application();
        Application(std::string appName, int cmdDepth_, int combineOpts_, int helpAvailability_);
        std::string appName;
        std::shared_ptr<Command> mainCommand;
        std::shared_ptr<Command> helpCommand;
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
        Application &addParameter(const std::string &name, const std::string &shorthand,
            const std::string& desc, const std::string &expect);
        Application &addReqParameter(const std::string &name, const std::string &shorthand,
            const std::string& desc, const std::string &expect);
        Application &addDefParameter(const std::string &name, const std::string &shorthand,
            const std::string& desc, const std::string &expect, const std::string &defValue);
        Application &addFlag(const std::string &name, const std::string &shorthand, const std::string &desc);
    };

}
