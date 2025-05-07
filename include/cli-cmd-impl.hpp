#pragma once
#include <algorithm>
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
        return "   "  + name + std::string(std::max(1, 10 - static_cast<int>(name.size())), ' ') + desc;
    }

    INLINE void Command::addOption(const std::string& str, const std::string& desc)
    {
        if (str.empty())
            throw std::invalid_argument("command is empty ");
        if (desc.empty())
            throw std::invalid_argument("must be description for help");
        if (str[0] != '-')
            throw std::invalid_argument("options must start with hyphen, use subcommands instead");
    }

    INLINE void Command::execute()
    {
        if (!handler)
            std::cout << "Placeholder: command not set" << std::endl;
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
        root.add(Node("Options"));
        printTree(root);
    }

    INLINE void Command::initPositional(int start, const std::vector<std::string>& args)
    {
        positionalArgs.clear();
        for (size_t i = start; i < args.size(); i++)
            positionalArgs.push_back(args[i]);
    }

    INLINE void Command::setHandler(const Action& handler)
    {
        this->handler = handler;
    }

    INLINE Command* Subcategory::addSubcomand(const Action& func, std::string str, const std::string& desc)
    {
        if (str.empty())
            throw std::runtime_error("command is empty ");
        if (str[0] == '-')
            throw std::runtime_error("command can't start with hyphen, use options or flags instead");
        if (app->commandsMap.find(str) == app->commandsMap.end()) {
            commands.push_back(std::make_shared<Command>(func, str, desc));
            app->commandsMap.emplace(str, commands.back().get());
            return commands.back().get();
        } else throw std::runtime_error("command already exist: " + str);
    }

    INLINE Command* Category::addSubcomand(const Action& func, std::string str, const std::string& desc)
    {
        if (str.empty())
            throw std::runtime_error("command is empty ");
        if (str[0] == '-')
            throw std::runtime_error("command can't start with hyphen, use options or flags instead");
        if (app->commandsMap.find(str) == app->commandsMap.end()) {
            commands.push_back(std::make_shared<Command>(func, str, desc));
            app->commandsMap.emplace(str, commands.back().get());
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

    INLINE void Application::parse(const std::vector<std::string>& args)
    {
        auto it = commandsMap.find("help");
        if (it == commandsMap.end())
            throw std::runtime_error("help not exists, use app.initHelp();");
        if (args.size() < 2)
        {
            mainCommand->execute();
            return;
        }
        it = commandsMap.find(args[1]);
        if (it == commandsMap.end())
        {
            std::cout << appName << ": '" << args[1] << "' is not a valid command see " <<
                appName << " --help" << std::endl ;
            auto mostSimilar = findMostSimilar(args[1], commandsMap);
            if (!mostSimilar.empty()) {
                if (mostSimilar.size() > 1)
                    std::cout << "The most similar commands are" << std::endl;
                else
                    std::cout << "The most similar command is" << std::endl;
                for (const auto& similar : mostSimilar)
                    std::cout << "     " << similar << std::endl;
            }
        } else {
            Command* command = it->second;
            command->initPositional(2, args);
            size_t actualPositionalArgsCount = args.size() - 2;
            if (actualPositionalArgsCount < command->positionalLimit.min)
                std::cout << appName << ": " << args[1] << " have " << actualPositionalArgsCount <<
                    " arguments but minimal is " << command->positionalLimit.min << std::endl;
            else if (actualPositionalArgsCount > command->positionalLimit.max)
                std::cout << appName << ": " << args[1] << " have " << actualPositionalArgsCount <<
                    " arguments but maximal is " << command->positionalLimit.max << std::endl;
            else if (!command)
            {
                std::cout << appName << ": '" << args[1] << "is placeholder with positional arguments:";
                for (const auto& arg : command->positionalArgs)
                    std::cout << arg << " ";
            } else
            {
                if (diagnostic == 1)
                    command->print();
                else
                    command->execute();
            }
        }
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
        if (commandsMap.find(str) == commandsMap.end()) {
            std::shared_ptr<Command> command = std::make_shared<Command>(func, str, desc);
            command->app = this;
            commands.push_back(command);
            commandsMap.emplace(str, commands.back().get());
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
    INLINE void Application::help(Command*) const
    {
        const bool bAll = true;

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

        Action actionHelp = [](const Application* app, Command* cmd) {
            app->help(cmd);
        };
        addSubcomand(actionHelp, "help", "Display help information about " + appName);
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

    INLINE std::vector<std::string> Application::findMostSimilar(const std::string& command, const std::map<std::string, Command*> &commands)
    {
        constexpr int maxDist = 5;
        std::vector<std::string> result;
        int bestLen = maxDist;
        for (const auto& other : commands)
        {
            cli ::Distance dist(command, other.first);
            int d = dist.compare();
            if (d <= bestLen)
            {
                if (d < bestLen)
                {
                    result.clear();
                    bestLen = d;
                }
                result.push_back(other.first);
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
