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
                    {"argument", v.argument},
                    {"min-n", v.min_n},
                    {"max-n", v.max_n},
        };
    }

    INLINE void to_json(json& j, const Flag& f) {
        j = json{
                    {"name", f.name},
            };
            if (!f.desc.empty())
                j["desc"] = f.desc;
    }

    INLINE void to_json(json& j, const Actual& a) {
        j = json{
                {"command", a.m_name},
                {"ignored_flags", a.ignoredFlags},
                {"flag_set", std::vector<std::string>(a.flagSet.begin(), a.flagSet.end())},
                {"error_number", a.errNumber},
            };
        if (a.errorStr.has_value()) {
            j["error_str"] = *a.errorStr;
        }
        j["arguments"] = a.arguments;
    }

    INLINE void to_json(json& j, const Formal& f) {
        // serialize flag map (flattened to list)
        json flags = json::array();
        for (const auto& [name, flagPtr] : f.availableFlagMap) {
            if (flagPtr) {
                flags.push_back(*flagPtr.get()); // requires to_json(json&, const Flag&)
            }
        }

        j = json{
            {"flags", flags},
            {"arguments", f.argList}, // requires to_json(json&, const Argument&)
        };

        if (f.vaArgs) {
            j["varargs"] = *f.vaArgs; // requires to_json(json&, const VaArguments&)
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

    inline Command& Command::desc(const std::string& _desc)
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
        Argument argument(std::move(name), std::move(type));
        VaArguments argVa(std::move(argument), min_n, max_n);
        this->formal.vaArgs = argVa;
        return *this;
    }

    INLINE Command& Command::addArgs(std::string name, std::string type, size_t min_n)
    {
        return addArgs(std::move(name), std::move(type), min_n, std::numeric_limits<size_t>::max());
    }

    INLINE Command& Command::addFlag(const std::string& name, const std::string& /*shorthand*/, const std::string& desc)
    {
        if (name.empty())
            throw std::invalid_argument("command is empty ");
        if (name[0] != '-')
            throw std::invalid_argument("flags must start with hyphen, use subcommands instead");
        if (formal.availableFlagMap.find(name) != formal.availableFlagMap.end()) {
            throw std::invalid_argument(fmt("flag %s already exists for command %s",
                name.c_str(), m_name.c_str()));
        }
        auto flag = std::make_shared<Flag>(name, desc);
        formal.availableFlagMap[name] = flag;
        return *this;
    }

    INLINE void Command::execute()
    {
        if (!ignoredFlags.empty())
        {
            std::cout << "ignored flags: [";
            for (size_t i = 0; i < ignoredFlags.size(); ++i)
            {
                if (i>0)
                    std::cout << " ";
                std::cout << ignoredFlags[i];
            }
            std::cout << "]" << std::endl;
        }
        if (errNumber)
        {
            if (errorStr)
                std::cout << *errorStr << std::endl;
            if (errNumber == ErrorCode::MissingHandler)
                for (const auto& arg : arguments)
                    std::cout << arg.value << " = [" << arg.argument.name << ":" << arg.argument.type << "]\n";
            if (!mostSimilar.empty()) {
                if (mostSimilar.size() > 1)
                    std::cout << "The most similar commands are" << std::endl;
                else
                    std::cout << "The most similar command is" << std::endl;
                for (const auto& similar : mostSimilar)
                    std::cout << "     " << similar << std::endl;
            }
        }
        else
        {
            m_handler(this);
        }
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
        if (formal.vaArgs.has_value())
        {
            positional = root.add(Node(fmt("VarPositional args: (min: %d, max: %d)",
                        formal.vaArgs.value().min_n, formal.vaArgs.value().max_n)));
            positional->add(Node(formal.vaArgs.value().argument.name + " : " + formal.vaArgs.value().argument.type));
        }
        Node* flags = root.add(Node("Flags"));
        for (const auto& arg: flagSet)
        {
            flags->add(Node(arg));
        }
        printTree(root);
    }

    inline Command& Command::addParameter(const std::string& /*name*/, const std::string& /*shorthand*/, const std::string& /*expect*/,
        const std::string& /*desc*/)
    {
        throw std::logic_error("not implemented");
    }

    inline std::string Flag::to_string() const
    {
        std::string indent(3, ' ');
        std::string result =  indent + name + std::string(std::max(1, 10 - static_cast<int>(name.size())), ' ') + desc;
        return result;
    }

    INLINE void Command::parse(int start, const std::vector<std::string>& args)
    {
        arguments.clear();
        size_t count = 0, varCount = 0;
        for (size_t i = start; i < args.size(); i++)
        {
            auto arg = args[i];
            assert(!arg.empty());
            if (arg[0]=='-')
            {
                if (formal.availableFlagMap.find(arg)  != formal.availableFlagMap.end())
                    flagSet.insert(arg);
                else
                    ignoredFlags.push_back(arg);
            }
            else
            {
                if (count < formal.argList.size())
                {
                    Argument &formalArgument = formal.argList[count++];
                    arguments.emplace_back(formalArgument, arg);
                } else
                {
                    Argument &formalArgument = formal.vaArgs->argument;
                    arguments.emplace_back(formalArgument, arg);
                    varCount++;
                }
            }
        }
        if (arguments.size() < formal.argList.size() + formal.vaArgs->min_n)
        {
            errNumber = ErrorCode::TooFewArguments;
            errorStr = fmt(ErrorMessage::TooFewArguments,
                app->appName.c_str(), m_name.c_str(), arguments.size(),
                formal.argList.size() + formal.vaArgs->min_n);
        }
        else if (arguments.size() > formal.argList.size() + formal.vaArgs->max_n)
        {
            errNumber = ErrorCode::TooManyArguments;
            errorStr = fmt(ErrorMessage::TooManyArguments,
                            app->appName.c_str(), m_name.c_str(), arguments.size(),
                            formal.argList.size() + formal.vaArgs->max_n);
        }
        else if (!m_handler)
        {
            errNumber = ErrorCode::MissingHandler;
            errorStr = fmt(ErrorMessage::MissingHandler, app->appName.c_str(),m_name.c_str());
        }
    }

    INLINE bool Category::isAlphaNumOrDash(const std::string& str) {
        return std::all_of(str.begin(), str.end(), [](unsigned char c) {
            return std::isalnum(c) || c == '-';
        });
    }

    INLINE  void Category::checkCommandName(const std::string& commandName)
    {
        if (commandName.empty())
            throw std::runtime_error("command is empty ");
        if (commandName[0] == '-')
            throw std::runtime_error(fmt("command can't start with dash, use options or flags instead",
                commandName.c_str()));
        if (commandName.find(' ') != std::string::npos)
            throw std::runtime_error(fmt("command [%s] can't contains spaces", commandName.c_str()));
        if (!isAlphaNumOrDash(commandName))
            throw std::runtime_error(fmt("command [%s] can't contains other characters than alpha-num and dash", commandName.c_str()));
    }

    INLINE Command& Category::addCommand(std::string commandName)
    {
        checkCommandName(commandName);
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
        checkCommandName(commandName);
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
                Argument argument("command","");
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
        parse(splitStringWithQuotes(line));
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

    INLINE Command& Application::addCommand(std::string name)
    {
        if (commandMap.find(name) == commandMap.end()) {
            std::shared_ptr<Command> command = std::make_shared<Command>(name, this);
            command->app = this;
            commands.push_back(command);
            commandMap.emplace(name, commands.back());
            return *commands.back().get();
        } else throw std::runtime_error("command already exist: " + name);
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
            auto* cmd = dynamic_cast<cli::Command*>(actual);
            cmd->commandNotFound(arg.value);
        }
        auto cmd = it->second;
        std::cout << cmd->to_string() << std::endl;
        for (const auto& [key, opt] : cmd->formal.availableFlagMap) {
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
                .addArgs("command", "", 0, 1);
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
        setArg(args, "diagnostic", diagnostic, 0, 1);
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
