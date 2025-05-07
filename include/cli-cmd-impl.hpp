#pragma once
#include <algorithm>
#include <cassert>
#include <sstream>
#include <unordered_map>

#include "cli-cmd.h"
#include "distance-impl.hpp"
#include "util-impl.hpp"

namespace cli
{
    INLINE void Command::setPositionalArgsLimits(size_t min, size_t max) {
        positionalLimit = {min, max};
    }

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

    INLINE void Command::addOption(const std::string& str, const std::string& desc)
    {
        if (str.empty())
            throw std::invalid_argument("command is empty ");
        if (desc.empty())
            throw std::invalid_argument("must be description for help");
        if (str[0] != '-')
            throw std::invalid_argument("options must start with hyphen, use subcommands instead");
        if (availableOptionMap.find(str) != availableOptionMap.end()) {
            throw std::invalid_argument(fmt("option %s already exists for command %s",
                str.c_str(), name.c_str()));
        }
        auto option = std::make_shared<Option>(str, desc);
        availableOptionMap[str] = option;
    }

    INLINE void Command::execute()
    {
        if (!ignoredOptions.empty())
        {
            std::cout << "ignored options: [";
            for (size_t i = 0; i < ignoredOptions.size(); ++i)
            {
                if (i>0)
                    std::cout << " ";
                std::cout << ignoredOptions[i];
            }
            std::cout << "]" << std::endl;
        }
        if (!handler)
            std::cout << "Placeholder for [" << name << "]: command not set" << std::endl;
        else
            handler(app, this);
    }

    INLINE void Command::print() const
    {
        Node root(fmt("Command [%s]", name.c_str()));
        Node* positional = root.add(Node(fmt("Positional args: (min: %d, max: %d)",
            positionalLimit.min, positionalLimit.max)));
        for (const auto& arg: positionalArgs)
        {
            positional->add(Node(arg));
        }
        Node* options = root.add(Node("Options"));
        for (const auto& arg: optionSet)
        {
            options->add(Node(arg));
        }
        printTree(root);
    }

    inline std::string Option::to_string() const
    {
        std::string indent(3, ' ');
        std::string result =  indent + name + std::string(std::max(1, 10 - static_cast<int>(name.size())), ' ') + desc;
        return result;
    }

    INLINE void Command::parse(int start, const std::vector<std::string>& args)
    {
        positionalArgs.clear();
        for (size_t i = start; i < args.size(); i++)
        {
            auto arg = args[i];
            assert(!arg.empty());
            if (arg[0]=='-')
            {
                if (availableOptionMap.find(arg)  != availableOptionMap.end())
                    optionSet.insert(arg);
                else
                    ignoredOptions.push_back(arg);
            }
            else
                positionalArgs.push_back(args[i]);
        }

        size_t actualPositionalArgsCount = positionalArgs.size();
        if (actualPositionalArgsCount < positionalLimit.min)
            std::cout << app->appName << ": " << args[1] << " have " << actualPositionalArgsCount <<
                " arguments but minimal is " << positionalLimit.min << std::endl;
        else if (actualPositionalArgsCount > positionalLimit.max)
            std::cout << app->appName << ": " << args[1] << " have " << actualPositionalArgsCount <<
                " arguments but maximal is " << positionalLimit.max << std::endl;
        else if (!handler)
        {
            std::cout << app->appName << ": '" << args[1] << "is placeholder with positional arguments:";
            for (const auto& arg : positionalArgs)
                std::cout << arg << " ";
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
        this->handler = handler;
    }

    inline bool Command::containsOption(const std::string& opt)
    {
        return optionSet.find(opt) != optionSet.end();
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
                cmdHelp->positionalArgs.clear();
                cmdHelp->positionalArgs.push_back(appName);
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
        const bool bAll = cmdHelp->containsOption("--all");

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
        auto arg = cmdHelp->positionalArgs[0];
        auto it = commandMap.find(arg);
        if (it == commandMap.end())
        {
            std::cout << fmt("command [%s] not exists", arg.c_str()) << std::endl;
            proposeSimilar(arg);
        }
        auto cmd = it->second;
        std::cout << cmd->to_string() << std::endl;
        for (const auto& [key, opt] : cmd->availableOptionMap) {
            std::cout << opt->to_string() << std::endl;
        }
    }


    INLINE void Application::help(Command* cmdHelp) const
    {
        if (cmdHelp->positionalArgs.empty())
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
        Action actionStub = [](Application* app, Command* cmd) {
            app->mainCommandStub(cmd);
        };
        mainCommand = std::make_shared<Command>(actionStub, appName, appName);
        mainCommand->app = this;
        commands.push_back(mainCommand);
        commandMap.emplace(appName, commands.back().get());

        Action actionHelp = [](const Application* app, Command* cmd) {
            app->help(cmd);
        };
        auto helpCmd = addSubcomand(actionHelp, "help", "Display help information about " + appName);
        helpCmd->setPositionalArgsLimits(0, 1);
        if (cmdDepth==3)
            helpCmd->addOption("--all", "all commands");
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
