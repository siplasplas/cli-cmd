#pragma once
#include <sstream>
#include <unordered_map>

#include "cli-cmd.h"
#include "distance-impl.hpp"
namespace cli
{
    INLINE void Command::setPositionalArgsLimits(int min, int max) {
        positionalLimit = {min, max};
    }

    INLINE std::string Command::to_string()
    {
        return "   "  + name + std::string(std::max(1, 10 - (int)name.size()), ' ') + desc;
    }

    INLINE void Command::initPositional(int start, const std::vector<std::string>& args)
    {
        positionalArgs.clear();
        for (size_t i = start; i < args.size(); i++)
            positionalArgs.push_back(args[i]);
    }

    INLINE Command* Subcategory::addSubcomand(std::function<void(cli::Application*, Command* command)> func, std::string str, const std::string desc)
    {
        if (str.empty())
            throw std::runtime_error("command is empty ");
        if (str[0] == '-')
            throw std::runtime_error("command can't start with hyphen, use options or flags instead");
        if (app->commandsMap.find(str) == app->commandsMap.end()) {
            std::unique_ptr<Command> command = std::make_unique<Command>(func, str, desc);
            commands.push_back(std::move(command));
            app->commandsMap.emplace(str, commands.back().get());
            return commands.back().get();
        } else throw std::runtime_error("command already exist: " + str);
    }

    INLINE Command* Category::addSubcomand(std::function<void(cli::Application*, Command* command)> func, std::string str, const std::string desc)
    {
        if (str.empty())
            throw std::runtime_error("command is empty ");
        if (str[0] == '-')
            throw std::runtime_error("command can't start with hyphen, use options or flags instead");
        if (app->commandsMap.find(str) == app->commandsMap.end()) {
            std::unique_ptr<Command> command = std::make_unique<Command>(func, str, desc);
            commands.push_back(std::move(command));
            app->commandsMap.emplace(str, commands.back().get());
            return commands.back().get();
        } else throw std::runtime_error("command already exist: " + str);
    }

    INLINE void Subcategory::addOption(std::string str, const std::string& desc)
    {
        if (str.empty())
            throw std::invalid_argument("command is empty ");
        if (desc.empty())
            throw std::invalid_argument("must be subcategory description for help");
        if (str[0] != '-')
            throw std::invalid_argument("options must start with hyphen, use subcommands instead");
    }

    INLINE std::string Subcategory::to_string()
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
        return name;
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
                args[token] = ""; // Flaga bez wartości
            }
        }
        for (auto arg: args)
            if (arg.second.empty())
                throw std::invalid_argument("parseSimpleArgs: "+ arg.first +
                    " is not a valid option for input: [" + input + "]");
            else
            {
                std::size_t pos;
                try
                {
                    std::stoi(arg.second, &pos);
                } catch (const std::invalid_argument& e)
                {
                    throw std::invalid_argument("parseSimpleArgs: for "+ arg.first +
                    "=" + arg.second + " is not number for input [" + input + "]");
                }
                if (pos != arg.second.size())
                    throw std::invalid_argument("parseSimpleArgs: for "+ arg.first +
                    "=" + arg.second + " is not number for input [" + input + "]");
                if (arg.first != "cmdDepth" && arg.first != "combineOpts")
                    throw std::invalid_argument("parseSimpleArgs: unknown "+ arg.first +
                    " for input [" + input + "]");
            }

        return args;
    }

    INLINE Category* Application::addCategory(std::string caption)
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
            help(this, it->second);
            return;
        }
        it = commandsMap.find(args[1]);
        if (it == commandsMap.end())
        {
            std::cout << appName << ": '" << args[1] << "' is not a valid command see " <<
                appName << " --help" << std::endl ;
            auto similars = most_similar_commands(args[1], commandsMap);
            if (similars.size() > 0) {
                if (similars.size() > 1)
                    std::cout << "The most similar commands are" << std::endl;
                else
                    std::cout << "The most similar command is" << std::endl;
                for (const auto& similar : similars)
                    std::cout << "     " << similar << std::endl;
            }
        } else {
            std::string name = args[1];
            Command* command = it->second;
            command->initPositional(2, args);
            int actualPositionalArgsCount = args.size() -2;
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
                command->execute();
        }
    }

    INLINE void Application::parse(std::string line)
    {
        parse(splitStringWithQuotes(line));
    }

    INLINE void Application::run(int argc, char** argv)
    {
        std::vector<std::string> args(argv, argv + argc);
        parse(args);
    }


    INLINE Command* Application::addSubcomand(std::function<void(Application*, Command* command)> func, std::string str, const std::string desc)
    {
        if (commandsMap.find(str) == commandsMap.end()) {
            std::unique_ptr<Command> command = std::make_unique<Command>(func, str, desc);
            command->app = this;
            commands.push_back(std::move(command));
            commandsMap.emplace(str, commands.back().get());
            return commands.back().get();
        } else throw std::runtime_error("command already exist: " + str);
    }

    INLINE void Application::help(Application*, Command*) {
        size_t subCount = 0;
        for (const auto& category_ptr : categories) {
            subCount += category_ptr->subcategories.size();
        }
        for (const auto& category_ptr : categories) {
            auto &subcategories = category_ptr->subcategories;
            for (const auto& subcategory_ptr : subcategories)
            {
                if (subCount > 1)
                    std::cout << subcategory_ptr->name << std::endl << std::endl;
                std::cout << subcategory_ptr->to_string() << std::endl;
            }
        }
    }

    INLINE Application::Application(std::string appName, std::string namedParams): appName(std::move(appName))
    {
        if (!appName.empty())
            throw std::invalid_argument("appName is empty");
        auto args = parseSimpleArgs(namedParams);
        cmdDepth =stoi( args["cmdDepth"]);
        if (cmdDepth < 0 || cmdDepth > 3)
            throw std::invalid_argument("cmdDepth must be between 0 and 3");
    }

    INLINE std::vector<std::string> Application::most_similar_commands(std::string command, const std::map<std::string, Command*> &commands) const
    {
        const int maxDist = 5;
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
            char ch = iss.peek();

            if ((ch == '"' || ch == '\'') && !inQuotes) {
                // Początek cudzysłowu
                inQuotes = true;
                currentQuote = ch;
                iss.get(); // Pobierz znak cudzysłowu
                std::string quotedToken;

                while (iss.get(ch) && ch != currentQuote) {
                    quotedToken += ch;
                }

                result.push_back(quotedToken);
                inQuotes = false;
            } else {
                // Normalny token (bez cudzysłowu)
                std::string normalToken;
                iss >> normalToken;
                result.push_back(normalToken);
            }
        }

        return result;
    }
}