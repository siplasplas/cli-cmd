#pragma once
#include <algorithm>
#include <cassert>
#include <iostream>
#include <limits>
#include <sstream>
#include <unordered_map>
#include <utility>

#include "cli-cmd.h"
#include "distance.h"
#include "error_codes.h"
#include "util.h"

namespace cli
{
    INLINE void to_json(json& j, const ArgumentValue& v) {
        j = json{
                {"name", v.argument.name},
                {"type", v.argument.type},
                {"value", v.value}
        };
    }

    INLINE void to_json(json& j, const Argument& v) {
        j = json{
                        {"name", v.name},
                        {"type", v.type},
            };
    }

    INLINE void to_json(json& j, const VaArguments& v) {
        j = json{
                    {"name", v.name},
                    {"type", v.type},
                    {"min-n", v.min_n},
                    {"max-n", v.max_n},
        };
    }


    INLINE const char* to_string_flagmode(FlagMode p) {
        switch (p) {
            case FlagMode::Present:  return "Present";
            case FlagMode::Hidden:    return "Hidden";
            default: return "<unknown>";
        }
    }

    INLINE const char* to_string_parametermode(ParameterMode p) {
        switch (p) {
            case ParameterMode::Optional:  return "Optional";
            case ParameterMode::Required:  return "Required";
            case ParameterMode::Defaulted: return "Defaulted";
            case ParameterMode::Hidden:    return "Hidden";
            default: return "<unknown>";
        }
    }

    INLINE void to_json(json& j, const Flag& f) {
        j = json{
                    {"name", f.name()},
                    {"flagMode", to_string_flagmode(f.flagMode())},
            };
            if (!f.description().empty())
                j["desc"] = f.description();
    }

    INLINE void to_json(json& j, const Parameter& p) {
        j = json{
                    {"name", p.name()},
                    {"parameterMode", to_string_parametermode(p.parameterMode())},
                    {"expect", p.expect()},
                    {"defValue", p.defValue()},
                };
        if (!p.description().empty())
            j["desc"] = p.description();
    }

    INLINE void to_json(json& j, const Actual& a) {
        j = json{
            {"command", a.m_name},
            {"flag_set", a.flagSet},
            {"parameter_map", a.parameterMap},
            };
        if (a.errNumber)
            j["error"] = to_string_errorCode(a.errNumber);
        j["arguments"] = a.arguments;
    }

    INLINE void to_json(json& j, const Formal& f) {
        // serialize flag map (flattened to list)
        json flags = json::array();
        json parameters = json::array();
        for (const auto& [name, ptr] : f.optionMap) {
            assert(ptr);
            auto rawPtr = ptr.get();
            if (ptr->kind() == OptionKind::Flag)
            {
                auto flagPtr = dynamic_cast<Flag*>(rawPtr);
                flags.push_back(*flagPtr);
            } else if (ptr->kind() == OptionKind::Parameter)
            {
                auto parameterPtr = dynamic_cast<Parameter*>(rawPtr);
                parameters.push_back(*parameterPtr);
            }
        }

        j = json{
            {"flags", flags},
            {"parameters", parameters},
            {"arguments", f.argList}, // requires to_json(json&, const Argument&)
        };

        if (f.vaArgs.max_n > 0) {
            j["varargs"] = f.vaArgs; // requires to_json(json&, const VaArguments&)
        }
    }

    INLINE std::optional<std::string> Actual::getParamValue(const std::string&)
    {
        throw std::logic_error("Not implemented");
    }

    INLINE bool Actual::containsFlag(const std::string& opt) const
    {
        return flagSet.find(opt) != flagSet.end();
    }

    INLINE void Actual::clearActual() {
        arguments.clear();
        flagSet.clear();
        errNumber = 0;
        errorStr = "";
        mostSimilar.clear();
    }

    INLINE void Formal::checkNames(Application *app, const std::string &name, const std::string &shorthand) {
        std::string errStr;
        if (app->combineOpts || !shorthand.empty())
            errStr = tokenError(name, ArgType::LongOption, app->combineOpts);
        else
            errStr = tokenError(name, {ArgType::LongOption, ArgType::GccOption}, app->combineOpts);
        if (!errStr.empty())
            throw std::invalid_argument(errStr);
        errStr = tokenError(shorthand, {ArgType::ShortOption, ArgError::InvalidEmpty}, app->combineOpts);
        if (!errStr.empty())
            throw std::invalid_argument(errStr);
    }

    INLINE void Formal::addShorthand(Application *app, const std::string &name, const std::string &shorthand) {
        if (!shorthand.empty())
        {
            auto it = app->shorthandMap.find(shorthand);
            if (it != app->shorthandMap.end())
                throw std::invalid_argument(fmt("shorthand %s already taken for option %s", shorthand.c_str(), it->second.c_str()));
            app->shorthandMap[shorthand] = name;
        }
    }

    INLINE void Formal::addFlag(Application *app, const std::string &name, const std::string &shorthand,
                                const std::string &desc)
    {
        checkNames(app, name, shorthand);
        addShorthand(app, name, shorthand);
        auto flag = std::make_shared<Flag>(name, desc, FlagMode::Present);
        optionMap[name] = flag;
    }

    INLINE void Formal::addParameter(Application *app, const std::string &name, const std::string &shorthand,
        const std::string &defValue, const std::string &expect, ParameterMode parameterMode, const std::string &desc)
    {
        checkNames(app, name, shorthand);
        addShorthand(app, name, shorthand);
        auto parameter = std::make_shared<Parameter>(name, desc, defValue, expect, parameterMode);
        optionMap[name] = parameter;
    }

    INLINE json Command::asJson()
    {
        json j = *this;
        return j;
    }

    INLINE json Command::formalAsJson()
    {
        json j = formal;
        return j;
    }

    INLINE void Command::commandNotFound(const std::string &arg)
    {
        errNumber = ErrorCode::UnknownCommand;
        errorStr = fmt(ErrorMessage::UnknownCommand,
            app->appName.c_str(), m_name.c_str(), app->appName.c_str(), app->appName.c_str());
        mostSimilar = app->proposeSimilar(arg);
    }

    INLINE std::string Command::to_string() const
    {
        std::string indent(3, ' ');
        std::string result =  indent + m_name + std::string(std::max(1, 10 - static_cast<int>(m_name.size())), ' ') + m_desc;
        if (m_name == "help")
        {
            if (app->cmdDepth == 3)
            {
                std::string blank(10, ' ');
                result += "\n" + indent + blank + "--all : print all commands";
            }
        }
        return result;
    }

    INLINE Command& Command::handler(const Action& _handler)
    {
        m_handler = _handler;
        return *this;
    }

    INLINE Command& Command::desc(const std::string& _desc)
    {
        m_desc = _desc;
        return *this;
    }

    INLINE Command& Command::addArg(std::string name, std::string type)
    {
        Argument argument(std::move(name), std::move(type));
        formal.argList.push_back(argument);
        return *this;
    }

    INLINE Command& Command::addArgs(std::string name, std::string type, size_t min_n, size_t max_n)
    {
        VaArguments argVa(std::move(name), std::move(type), min_n, max_n);
        this->formal.vaArgs = argVa;
        return *this;
    }

    INLINE Command& Command::addArgs(std::string name, std::string type, size_t min_n)
    {
        return addArgs(std::move(name), std::move(type), min_n, std::numeric_limits<size_t>::max());
    }

    INLINE Command& Command::addFlag(const std::string& name, const std::string& shorthand, const std::string& desc)
    {
        formal.addFlag(app, name, shorthand, desc);
        return *this;
    }

    INLINE void Command::printSimilars() {
        if (!mostSimilar.empty()) {
            if (mostSimilar.size() > 1)
                std::cout << "The most similar commands are" << std::endl;
            else
                std::cout << "The most similar command is" << std::endl;
            for (const auto& similar : mostSimilar)
                std::cout << "     " << similar << std::endl;
        }
    }

    INLINE void Command::printErrors() {
        if (errorStr)
            std::cout << *errorStr << std::endl;
        if (errNumber == ErrorCode::MissingHandler)
            for (const auto& arg : arguments)
                std::cout << arg.value << " = [" << arg.argument.name << ":" << arg.argument.type << "]\n";
        else if (errNumber == ErrorCode::UnknownCommand) {
            auto cmdHelp = app->getCommand("help");
            printSimilars();
        }
    }

    INLINE void Command::execute()
    {
        if (errNumber)
            printErrors();
        else
            m_handler(this);
    }

    INLINE void Command::print() const
    {
        Node root(fmt("Command [%s]", m_name.c_str()));
        Node* positional = root.add(Node(fmt("Positional args: (size: %d)",
            formal.argList.size())));
        for (const auto& arg: formal.argList)
        {
            positional->add(Node(arg.name + " : " + arg.type));
        }
        if (formal.vaArgs.max_n > 0)
        {
            positional = root.add(Node(fmt("VarPositional args: (min: %d, max: %d)",
                        formal.vaArgs.min_n, formal.vaArgs.max_n)));
            positional->add(Node(formal.vaArgs.name + " : " + formal.vaArgs.type));
        }
        Node* flags = root.add(Node("Flags"));
        for (const auto& arg: flagSet)
        {
            flags->add(Node(arg));
        }
        printTree(root);
    }

    INLINE Command& Command::addParameter(const std::string& name, const std::string& shorthand,
        const std::string& expect, const std::string& desc)
    {
        formal.addParameter(app, name, shorthand, "", expect, ParameterMode::Optional, desc);
        return *this;
    }

    INLINE Command& Command::addReqParameter(const std::string& name, const std::string& shorthand,
        const std::string& expect, const std::string& desc)
    {
        formal.addParameter(app, name, shorthand, "", expect, ParameterMode::Required, desc);
        return *this;
    }

    INLINE Command& Command::addDefParameter(const std::string& name, const std::string& shorthand,
        const std::string& defValue, const std::string& expect, const std::string& desc)
    {
        formal.addParameter(app, name, shorthand, defValue, expect, ParameterMode::Defaulted, desc);
        return *this;
    }

    INLINE std::string Flag::to_string() const
    {
        std::string indent(3, ' ');
        std::string result =  indent + name() + std::string(std::max(1, 10 - static_cast<int>(name().size())), ' ')
                + description();
        return result;
    }

    INLINE std::string Parameter::to_string() const {
        std::string indent(3, ' ');
        std::string result =  indent + name() + std::string(std::max(1, 10 - static_cast<int>(name().size())), ' ')
                + description();
        return result;
    }

    INLINE void Command::parse(int start, const std::vector<std::string>& args)
    {
        clearActual();
        size_t count = 0, varCount = 0;
        size_t argNumber = start;
        while (argNumber < args.size())
        {
            auto arg = args[argNumber];
            std::vector<int> expectedClasses = {BareIdentifier, Freeform};
            expectedClasses.insert(expectedClasses.end(), {ShortOption, LongOption});
            auto errStr = tokenError(arg, expectedClasses, app->combineOpts);
            if (!errStr.empty()) {
                errorStr = errStr;
                errNumber = ErrorCode::BadTokenForm;
                return;
            }
            auto tokenClass = classifyToken(arg, app->combineOpts);
            std::string optStr;
            switch (tokenClass) {
                case LongOption: case LongEquals:
                    optStr = arg;
                    break;
                case ShortOption: case ShortEquals: {
                    auto it = app->shorthandMap.find(arg);
                    if (it != app->shorthandMap.end()) {
                        optStr = it->second;
                    } else
                    {
                        errorStr = fmt(ErrorMessage::UnknownShortOption, arg.c_str());
                        errNumber = ErrorCode::UnknownShortOption;
                        return;
                    }
                    break;
                }
                default:; //positional
            }
            if (!optStr.empty()) {
                auto it = formal.optionMap.find(optStr);
                if (it  == formal.optionMap.end())
                {
                    errorStr = fmt(ErrorMessage::UnknownLongOption, optStr.c_str());
                    errNumber = ErrorCode::UnknownLongOption;
                    return;
                }
                auto opt = it->second.get();
                if (opt->kind() == OptionKind::Flag)
                    flagSet.insert(optStr);
                else if (opt->kind() == OptionKind::Parameter) {
                    auto parameter = dynamic_cast<Parameter*>(opt);
                    argNumber++;
                    if (argNumber >= args.size()) {
                        errorStr = fmt(ErrorMessage::UnexpectedCommandLineEnd, parameter->name().c_str());
                        errNumber = ErrorCode::UnexpectedCommandLineEnd;
                        return;
                    }
                    arg = args[argNumber];
                    parameterMap[parameter->name()] = arg;
                }
            }
            else
            {
                if (count < formal.argList.size())
                {
                    Argument &formalArgument = formal.argList[count++];
                    arguments.emplace_back(formalArgument, arg);
                } else
                {
                    Argument &formalArgument = formal.vaArgs;
                    arguments.emplace_back(formalArgument, arg);
                    varCount++;
                }
            }
            argNumber++;
        }
        if (arguments.size() < formal.argList.size() + formal.vaArgs.min_n)
        {
            errNumber = ErrorCode::TooFewArguments;
            errorStr = fmt(ErrorMessage::TooFewArguments,
                app->appName.c_str(), m_name.c_str(), arguments.size(),
                formal.argList.size() + formal.vaArgs.min_n);
        }
        else if (arguments.size() > formal.argList.size() + formal.vaArgs.max_n)
        {
            errNumber = ErrorCode::TooManyArguments;
            errorStr = fmt(ErrorMessage::TooManyArguments,
                            app->appName.c_str(), m_name.c_str(), arguments.size(),
                            formal.argList.size() + formal.vaArgs.max_n);
        }
        else if (!m_handler)
        {
            errNumber = ErrorCode::MissingHandler;
            errorStr = fmt(ErrorMessage::MissingHandler, app->appName.c_str(),m_name.c_str());
        }
    }

    INLINE Command& Category::addCommand(std::string commandName)
    {
        auto errStr = tokenError(commandName, ArgType::BareIdentifier);
        if (!errStr.empty())
            throw std::invalid_argument(errStr);
        if (app->commandMap.find(commandName) == app->commandMap.end()) {
            std::shared_ptr<Command> command = std::make_shared<Command>(commandName, app);
            command->app = app;
            commands.push_back(command);
            app->commandMap.emplace(commandName, commands.back());
            return *commands.back().get();
        } else throw std::runtime_error("command already exist: " + commandName);
    }

    INLINE Category& Category::ref(const std::string& commandName)
    {
        auto errStr = tokenError(commandName, ArgType::BareIdentifier);
        if (!errStr.empty())
            throw std::invalid_argument(errStr);
        auto it = app->commandMap.find(commandName);
        if (it != app->commandMap.end()) {
            commands.push_back(it->second);
            return *this;
        } else throw std::runtime_error("command not exist: " + commandName);
    }

    INLINE std::string Category::to_string()
    {
        return description;
    }

    INLINE std::unordered_map<std::string, std::string> Application::parseSimpleArgs(const std::string& input) {
        std::unordered_map<std::string, std::string> args;
        std::istringstream iss(input);
        std::string token;

        while (iss >> token) {
            size_t eq_pos = token.find('=');
            if (eq_pos != std::string::npos) {
                std::string key = token.substr(0, eq_pos);
                std::string value = token.substr(eq_pos + 1);
                args[key] = value;
            } else {
                args[token] = ""; // Flag without value
            }
        }
        for (const auto& arg: args)
            if (arg.second.empty())
                throw std::invalid_argument("parseSimpleArgs: "+ arg.first +
                    " is not a valid option for input: [" + input + "]");
            else
            {
                std::size_t pos;
                try
                {
                    std::stoi(arg.second, &pos);
                } catch (const std::invalid_argument&)
                {
                    throw std::invalid_argument("parseSimpleArgs: for "+ arg.first +
                    "=" + arg.second + " is not number for input [" + input + "]");
                }
                if (pos != arg.second.size())
                    throw std::invalid_argument("parseSimpleArgs: for "+ arg.first +
                    "=" + arg.second + " is not number for input [" + input + "]");
                if (arg.first != "cmdDepth" && arg.first != "combineOpts"
                    && arg.first != "helpAtStart" && arg.first != "diagnostic" )
                    throw std::invalid_argument("parseSimpleArgs: unknown "+ arg.first +
                    " for input [" + input + "]");
            }

        return args;
    }

    INLINE Category* Application::addCategory(const std::string& caption)
    {
        auto category = std::make_unique<Category>(caption, this);
        categories.push_back(std::move(category));
        return categories.back().get();
    }

    INLINE Category& Application::addHelpCategory(const std::string& caption)
    {
        auto category = std::make_unique<Category>(caption, this);
        helpCategories.push_back(std::move(category));
        return *helpCategories.back().get();
    }

    INLINE void Application::execute()
    {
        if (currentCommand)
            currentCommand->execute();
    }

    INLINE std::shared_ptr<Command> Application::getCommand(const std::string& name)
    {
        auto it = commandMap.find(name);
        if (it != commandMap.end()) {
            return it->second;
        } else
            throw std::runtime_error("command not found: " + name);
    }

    INLINE std::vector<std::string>  Application::proposeSimilar(const std::string& arg) const
    {
        std::vector<std::string> keys;
        keys.reserve(commandMap.size());
        std::transform(commandMap.begin(), commandMap.end(), std::back_inserter(keys),
                       [](const auto& pair) { return pair.first; });
        auto mostSimilar = findMostSimilar(arg, keys);
        return mostSimilar;
    }

    INLINE void Application::parse(const std::vector<std::string>& args)
    {
        auto it = commandMap.find("help");
        if (it == commandMap.end())
            throw std::runtime_error("help not exists");
        if (args.size() < 2)
        {
            if (mainCommand)
                currentCommand = mainCommand;
            else
            {   auto helpCommand = getCommand("help");
                currentCommand = helpCommand;
            }
            return;
        }
        int start = 1;
        if (cmdDepth == 0)
        {
            currentCommand = mainCommand;
            if (args[1] == "help")
            {
                auto cmdHelp = getCommand("help");
                cmdHelp->arguments.clear();
                Argument argument("command","identifier");
                ArgumentValue argValue(argument, appName);
                cmdHelp->arguments.push_back(argValue);
                help(cmdHelp.get());
            }
        }
        else
        {
            start = 2;
            it = commandMap.find(args[1]);
            if (it == commandMap.end())
            {
                currentCommand = std::make_shared<Command>(args[1], this);
                currentCommand->commandNotFound(args[1]);
            }
            else
            {
                currentCommand = it->second;
                currentCommand->parse(start, args);
            }
        }
    }

    INLINE void Application::parse(const std::string& line)
    {
        const auto args = splitStringWithQuotes(line);
        parse(args);
    }

    INLINE void Application::parse(int argc, char** argv)
    {
        std::vector<std::string> args(argv, argv + argc);
        parse(args);
    }

    INLINE void Application::run(int argc, char** argv)
    {
        parse(argc, argv);
        execute();
    }

    INLINE Command& Application::addCommand(std::string commandName)
    {
        auto errStr = tokenError(commandName, ArgType::BareIdentifier);
        if (!errStr.empty())
            throw std::invalid_argument(errStr);
        if (commandMap.find(commandName) == commandMap.end()) {
            std::shared_ptr<Command> command = std::make_shared<Command>(commandName, this);
            command->app = this;
            commands.push_back(command);
            commandMap.emplace(commandName, commands.back());
            return *commands.back().get();
        } else throw std::runtime_error("command already exist: " + commandName);
    }

    /**
     * @brief Display help message based on command hierarchy depth.
     *
     * Behavior depends on the value of `cmdDepth`:
     * - 0: Shows only top-level commands (GCC-style).
     * - 1: Flat list of commands defined in the Application.
     * - 2: Shows commands from Application and grouped by categories with descriptions.
     * - 3: If `bAll` is false, shows subcategories' descriptions and their commands.
     *      If `bAll` is true, shows all commands from categories and subcategories, sorted by name within categories.
     */
    INLINE void Application::printCommands(const Actual* actual) const
    {
        const bool bAll = actual->containsFlag("--all");

        for (const auto& cmd : commands) {
            std::cout << cmd->to_string() << std::endl;
        }

        if (cmdDepth == 2 || (cmdDepth == 3 && bAll)) {
            for (const auto& category_ptr : categories) {
                std::cout << std::endl << category_ptr->description << std::endl;
                for (const auto& cmd : category_ptr->commands) {
                    std::cout << cmd->to_string() << std::endl;
                }
            }
            return;
        } else if (cmdDepth == 3) {
            for (const auto& category_ptr : helpCategories)
            {
                std::cout << std::endl << category_ptr->description << std::endl;
                for (const auto& cmd : category_ptr->commands) {
                    std::cout << cmd->to_string() << std::endl;
                }
            }
        }
    }

    INLINE void Application::commandHelp(Actual* actual)
    {
        auto arg = actual->arguments[0];
        auto it = commandMap.find(arg.value);
        if (it == commandMap.end())
        {
            auto cmd = std::make_shared<Command>(arg.value, this);
            cmd->commandNotFound(arg.value);
            cmd->printErrors();
            return;
        }
        auto cmd = it->second;
        std::cout << cmd->to_string() << std::endl;
        for (const auto& [key, opt] : cmd->formal.optionMap) {
            std::cout << opt->to_string() << std::endl;
        }
    }


    INLINE void Application::help(Actual* actual)
    {
        if (actual->arguments.empty())
            printCommands(actual);
        else
            commandHelp(actual);
    }

    INLINE void Application::mainCommandStub(Actual* actual)
    {
        if (cmdDepth == 0)
        {
            std::cout << "stub command, use:" << std::endl << std::endl;
            std::cout << "  auto mainCommand = app.mainCommand;" << std::endl;
            std::cout << "  mainCommand->setHandler(mainHandler);" << std::endl;
        } else
            help(actual);
    }

    INLINE void Application::setArg(std::unordered_map<std::string, std::string> &args,
        const std::string& name, int &arg, int min, int max)
    {
        std::string value = args[name];
        if (value.empty())
            return;
        arg =stoi(value);
        if (arg < min || arg > max)
            throw std::invalid_argument(fmt("%s is %d and must be between %d and %d",
                name.c_str(), arg, min, max));
    }

    INLINE void Application::initSystemCommands()
    {
        if (cmdDepth == 0)
        {
            Action actionStub = [this](Actual* actual) {
                mainCommandStub(actual);
            };
            mainCommand = std::make_shared<Command>(appName, this);
            mainCommand->desc(appName);
            mainCommand->handler(actionStub);
            mainCommand->app = this;
            commands.push_back(mainCommand);
            commandMap.emplace(appName, commands.back());
        }

        Action actionHelp = [this](Actual* actual) {
            help(actual);
        };
        auto &helpCmd = addCommand("help").desc("Display help information about " + appName).handler(actionHelp)
                .addArgs("command", "identifier", 0, 1);
        if (cmdDepth==3)
            helpCmd.addFlag("--all", "", "all commands");
    }

    INLINE Application::Application(std::string appName, const std::string& namedParams): appName(std::move(appName))
    {
        if (!appName.empty())
            throw std::invalid_argument("appName is empty");
        auto args = parseSimpleArgs(namedParams);
        setArg(args, "cmdDepth", cmdDepth, 0, 3);
        setArg(args, "combineOpts", combineOpts, 0, 1);
        setArg(args, "helpAtStart", helpAtStart, 0, 1);
        initSystemCommands();
    }

    INLINE std::vector<std::string> Application::findMostSimilar(const std::string& proposed, const std::vector<std::string> &keys)
    {
        constexpr int maxDist = 5;
        std::vector<std::string> result;
        int bestLen = maxDist;
        for (const auto& key : keys)
        {
            cli ::Distance dist(proposed, key);
            int d = dist.compare();
            if (d <= bestLen)
            {
                if (d < bestLen)
                {
                    result.clear();
                    bestLen = d;
                }
                result.push_back(key);
            }
        }
        return result;
    }

    INLINE std::vector<std::string> Application::splitStringWithQuotes(const std::string& input) {
        std::vector<std::string> result;
        std::istringstream iss(input);
        std::string token;
        char currentQuote = '\0';

        while (iss >> std::ws) {
            char ch = static_cast<char>(iss.peek());

            if ((ch == '"' || ch == '\'')) {
                // The beginning of the quotation marks
                currentQuote = ch;
                iss.get(); // Download the quotation mark
                std::string quotedToken;

                while (iss.get(ch) && ch != currentQuote) {
                    quotedToken += ch;
                }

                result.push_back(quotedToken);
            } else {
                // Normal token (without quotation marks)
                std::string normalToken;
                iss >> normalToken;
                result.push_back(normalToken);
            }
        }

        return result;
    }
}
