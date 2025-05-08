#pragma once
#include <algorithm>
#include <cassert>
#include <sstream>
#include <unordered_map>
#include <utility>

#include "cli-cmd.h"
#include "distance-impl.hpp"
#include "util-impl.hpp"

namespace cli
{
    INLINE std::string Command::to_string() const
    {
        std::string indent(3, ' ');
        std::string result =  indent + name + std::string(std::max(1, 10 - static_cast<int>(name.size())), ' ') + desc;
        if (name == "help")
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

    INLINE Command& Command::addArg(std::string name, std::string type)
    {
        Argument argument(std::move(name), std::move(type));
        formalArgList.push_back(argument);
        return *this;
    }

    INLINE Command& Command::addArgs(std::string name, std::string type, size_t min_n, size_t max_n)
    {
        Argument argument(std::move(name), std::move(type));
        VaArguments argVa(std::move(argument), min_n, max_n);
        this->formalVaArgs = argVa;
        return *this;
    }

    INLINE Command& Command::addArgs(std::string name, std::string type, size_t min_n)
    {
        return addArgs(std::move(name), std::move(type), min_n, std::numeric_limits<size_t>::max());
    }

    INLINE void Command::addFlag(const std::string& str, const std::string& desc)
    {
        if (str.empty())
            throw std::invalid_argument("command is empty ");
        if (desc.empty())
            throw std::invalid_argument("must be description for help");
        if (str[0] != '-')
            throw std::invalid_argument("flags must start with hyphen, use subcommands instead");
        if (availableFlagMap.find(str) != availableFlagMap.end()) {
            throw std::invalid_argument(fmt("flag %s already exists for command %s",
                str.c_str(), name.c_str()));
        }
        auto flag = std::make_shared<Flag>(str, desc);
        availableFlagMap[str] = flag;
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
        if (!m_handler)
            std::cout << "Placeholder for [" << name << "]: command not set" << std::endl;
        else
            m_handler(app, this);
    }

    INLINE void Command::print() const
    {
        Node root(fmt("Command [%s]", name.c_str()));
        Node* positional = root.add(Node(fmt("Positional args: (size: %d)",
            formalArgList.size())));
        for (const auto& arg: formalArgList)
        {
            positional->add(Node(arg.name + " : " + arg.type));
        }
        if (formalVaArgs.has_value())
        {
            positional = root.add(Node(fmt("VarPositional args: (min: %d, max: %d)",
                        formalVaArgs.value().min_n, formalVaArgs.value().max_n)));
            positional->add(Node(formalVaArgs.value().argument.name + " : " + formalVaArgs.value().argument.type));
        }
        Node* flags = root.add(Node("Flags"));
        for (const auto& arg: flagSet)
        {
            flags->add(Node(arg));
        }
        printTree(root);
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
        size_t count = 0, vacount = 0;
        for (size_t i = start; i < args.size(); i++)
        {
            auto arg = args[i];
            assert(!arg.empty());
            if (arg[0]=='-')
            {
                if (availableFlagMap.find(arg)  != availableFlagMap.end())
                    flagSet.insert(arg);
                else
                    ignoredFlags.push_back(arg);
            }
            else
            {
                if (count < formalArgList.size())
                {
                    Argument &formalArgument = formalArgList[count++];
                    arguments.emplace_back(formalArgument, arg);
                } else
                {
                    Argument &formalArgument = formalVaArgs->argument;
                    arguments.emplace_back(formalArgument, arg);
                    vacount++;
                }
            }
        }
        if (arguments.size() < formalArgList.size() + formalVaArgs->min_n)
            std::cout << app->appName << ": " << name << " have " << arguments.size() <<
                " arguments but minimal is " << formalArgList.size() + formalVaArgs->min_n << std::endl;
        else if (arguments.size() > formalArgList.size() + formalVaArgs->max_n)
            std::cout << app->appName << ": " << name << " have " << arguments.size() <<
                " arguments but maximal is " << formalArgList.size() + formalVaArgs->max_n << std::endl;
        else if (!m_handler)
        {
            std::cout << app->appName << ": " << args[1] << " is placeholder with positional arguments:\n";
            for (const auto& arg : arguments)
                std::cout << arg.value << " = [" << arg.argument.name << ":" << arg.argument.type << "]\n";
        } else
        {
            if (app->diagnostic == 1)
                print();
            else
                execute();
        }
    }

    INLINE void Command::setHandler(const Action& handler)
    {
        this->m_handler = handler;
    }

    inline bool Command::containsFlag(const std::string& opt)
    {
        return flagSet.find(opt) != flagSet.end();
    }

    INLINE Command* Category::addSubcomand(const Action& func, std::string str, const std::string& desc)
    {
        if (str.empty())
            throw std::runtime_error("command is empty ");
        if (str[0] == '-')
            throw std::runtime_error("command can't start with hyphen, use options or flags instead");
        if (app->commandMap.find(str) == app->commandMap.end()) {
            std::shared_ptr<Command> command = std::make_shared<Command>(func, str, desc);
            command->app = app;
            commands.push_back(command);
            app->commandMap.emplace(str, commands.back().get());
            return commands.back().get();
        } else throw std::runtime_error("command already exist: " + str);
    }

    INLINE std::string Category::to_string()
    {
        return description;
    }

    INLINE Subcategory* Category::addSubcategory(std::string caption)
    {
        auto subcategory = std::make_unique<Subcategory>(std::move(caption), app);
        subcategories.push_back(std::move(subcategory));
        return subcategories.back().get();
    }

    INLINE Command* Subcategory::addSubcomand(const Action& func, std::string str, const std::string& desc)
    {
        if (str.empty())
            throw std::runtime_error("command is empty ");
        if (str[0] == '-')
            throw std::runtime_error("command can't start with hyphen, use options or flags instead");
        if (app->commandMap.find(str) == app->commandMap.end()) {
            std::shared_ptr<Command> command = std::make_shared<Command>(func, str, desc);
            command->app = app;
            commands.push_back(command);
            app->commandMap.emplace(str, commands.back().get());
            return commands.back().get();
        } else throw std::runtime_error("command already exist: " + str);
    }

    INLINE std::string Subcategory::to_string() const
    {
        std::string result;
        for (const auto& command_ptr : commands)
        {
            result += command_ptr->to_string() + "\n";
        }
        return result;
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

    INLINE Command* Application::getCommand(const std::string& name)
    {
        auto it = commandMap.find(name);
        if (it != commandMap.end()) {
            return it->second;
        } else
            throw std::runtime_error("command not found: " + name);
    }

    INLINE void Application::proposeSimilar(const std::string& arg) const
    {
        std::vector<std::string> keys;
        keys.reserve(commandMap.size());
        std::transform(commandMap.begin(), commandMap.end(), std::back_inserter(keys),
                       [](const auto& pair) { return pair.first; });
        auto mostSimilar = findMostSimilar(arg, keys);
        if (!mostSimilar.empty()) {
            if (mostSimilar.size() > 1)
                std::cout << "The most similar commands are" << std::endl;
            else
                std::cout << "The most similar command is" << std::endl;
            for (const auto& similar : mostSimilar)
                std::cout << "     " << similar << std::endl;
        }
    }

    INLINE void Application::commandNotFound(const std::string &arg)
    {
        std::cout << appName << ": '" << arg << "' is not a valid command see " <<
                appName << " --help" << std::endl ;
        proposeSimilar(arg);
    }

    INLINE void Application::parse(const std::vector<std::string>& args)
    {
        auto it = commandMap.find("help");
        if (it == commandMap.end())
            throw std::runtime_error("help not exists, use app.initHelp();");
        if (args.size() < 2)
        {
            mainCommand->execute();
            return;
        }
        Command* command = nullptr;
        int start = 1;
        if (cmdDepth == 0)
        {
            command = mainCommand.get();
            if (args[1] == "help")
            {
                auto cmdHelp = getCommand("help");
                cmdHelp->arguments.clear();
                Argument argument("command","");
                ArgumentValue avalue(argument, appName);
                cmdHelp->arguments.push_back(avalue);
                help(cmdHelp);
                return;
            }
        }
        else
        {
            start = 2;
            it = commandMap.find(args[1]);
            if (it == commandMap.end())
                commandNotFound(args[1]);
            else
                command = it->second;
        }
        if (command == nullptr)
            return;
        command->parse(start, args);
    }

    INLINE void Application::parse(const std::string& line)
    {
        parse(splitStringWithQuotes(line));
    }

    INLINE void Application::run(int argc, char** argv)
    {
        std::vector<std::string> args(argv, argv + argc);
        parse(args);
    }


    INLINE Command* Application::addSubcomand(const Action& func, const std::string& str, const std::string& desc)
    {
        if (commandMap.find(str) == commandMap.end()) {
            std::shared_ptr<Command> command = std::make_shared<Command>(func, str, desc);
            command->app = this;
            commands.push_back(command);
            commandMap.emplace(str, commands.back().get());
            return commands.back().get();
        } else throw std::runtime_error("command already exist: " + str);
    }

    INLINE Command& Application::addCommand(std::string name, const std::string& desc)
    {
        if (commandMap.find(name) == commandMap.end()) {
            std::shared_ptr<Command> command = std::make_shared<Command>(nullptr, name, desc);
            command->app = this;
            commands.push_back(command);
            commandMap.emplace(name, commands.back().get());
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
    INLINE void Application::printCommands(Command* cmdHelp) const
    {
        const bool bAll = cmdHelp->containsFlag("--all");

        if (cmdDepth <= 1) {
            for (const auto& cmd : commands) {
                std::cout << cmd->to_string() << std::endl;
            }
            return;
        }

        if (cmdDepth == 2) {
            for (const auto& cmd : commands) {
                std::cout << cmd->to_string() << std::endl;
            }

            for (const auto& category_ptr : categories) {
                std::cout << std::endl << category_ptr->description << std::endl;
                for (const auto& cmd : category_ptr->commands) {
                    std::cout << cmd->to_string() << std::endl;
                }
            }
            return;
        }

        if (cmdDepth == 3) {
            for (const auto& cmd : commands) {
                std::cout << cmd->to_string() << std::endl;
            }

            for (const auto& category_ptr : categories) {
                if (!bAll) {
                    for (const auto& subcategory_ptr : category_ptr->subcategories) {
                        std::cout << std::endl << subcategory_ptr->description << std::endl;
                        for (const auto& cmd : subcategory_ptr->commands) {
                            std::cout << cmd->to_string() << std::endl;
                        }
                    }
                } else {
                    std::vector<std::shared_ptr<Command>> all_cmds = category_ptr->commands;
                    for (const auto& subcategory_ptr : category_ptr->subcategories) {
                        all_cmds.insert(all_cmds.end(),
                                        subcategory_ptr->commands.begin(),
                                        subcategory_ptr->commands.end());
                    }

                    std::sort(all_cmds.begin(), all_cmds.end(),
                        [](const std::shared_ptr<Command>& a, const std::shared_ptr<Command>& b) {
                            return a->name < b->name;
                        });

                    std::cout << std::endl << category_ptr->description << std::endl;
                    for (const auto& cmd : all_cmds) {
                        std::cout << cmd->to_string() << std::endl;
                    }
                }
            }
        }
    }

    INLINE void Application::commandHelp(Command* cmdHelp) const
    {
        auto arg = cmdHelp->arguments[0];
        auto it = commandMap.find(arg.value);
        if (it == commandMap.end())
        {
            std::cout << fmt("command [%s] not exists", arg.value.c_str()) << std::endl;
            proposeSimilar(arg.value);
        }
        auto cmd = it->second;
        std::cout << cmd->to_string() << std::endl;
        for (const auto& [key, opt] : cmd->availableFlagMap) {
            std::cout << opt->to_string() << std::endl;
        }
    }


    INLINE void Application::help(Command* cmdHelp) const
    {
        if (cmdHelp->arguments.empty())
            printCommands(cmdHelp);
        else
            commandHelp(cmdHelp);
    }

    INLINE void Application::mainCommandStub(Command* command)
    {
        if (cmdDepth == 0)
        {
            std::cout << "stub command, use:" << std::endl << std::endl;
            std::cout << "  auto mainCommand = app.mainCommand;" << std::endl;
            std::cout << "  mainCommand->setHandler(mainHandler);" << std::endl;
        } else
            help(command);
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
            Action actionStub = [](Application* app, Command* cmd) {
                app->mainCommandStub(cmd);
            };
            mainCommand = std::make_shared<Command>(actionStub, appName, appName);
            mainCommand->app = this;
            commands.push_back(mainCommand);
            commandMap.emplace(appName, commands.back().get());
        }

        Action actionHelp = [](const Application* app, Command* cmd) {
            app->help(cmd);
        };
        auto helpCmd = addSubcomand(actionHelp, "help", "Display help information about " + appName);
        helpCmd->addArgs("command", "", 0, 1);
        if (cmdDepth==3)
            helpCmd->addFlag("--all", "all commands");
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
        bool inQuotes = false;
        char currentQuote = '\0';

        while (iss >> std::ws) {
            char ch = static_cast<char>(iss.peek());

            if ((ch == '"' || ch == '\'') && !inQuotes) {
                // The beginning of the quotation marks
                inQuotes = true;
                currentQuote = ch;
                iss.get(); // Download the quotation mark
                std::string quotedToken;

                while (iss.get(ch) && ch != currentQuote) {
                    quotedToken += ch;
                }

                result.push_back(quotedToken);
                inQuotes = false;
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
