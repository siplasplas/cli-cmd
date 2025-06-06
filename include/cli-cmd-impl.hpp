#pragma once
#include <algorithm>
#include <cassert>
#include <iostream>
#include <limits>
#include <sstream>
#include <utility>

#include "cli-cmd.h"
#include "distance.h"
#include "error_codes.h"
#include "util.h"
#include "validator.h"

namespace cli
{
    INLINE void to_json(json& j, const ArgumentValue& v) {
        j = json{
                {"name", v.argument.name()},
                {"expectType", v.argument.expectType()},
                {"value", v.value}
        };
    }

    INLINE void to_json(json& j, const Argument& v) {
        j = json{
                        {"name", v.name()},
                        {"expectType", v.expectType()},
            };
    }

    INLINE void to_json(json& j, const VaArguments& v) {
        j = json{
                    {"name", v.name()},
                    {"expectType", v.expectType()},
                    {"min-n", v.min_n},
                    {"max-n", v.max_n},
        };
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
            };
            if (!f.description().empty())
                j["desc"] = f.description();
    }

    INLINE void to_json(json& j, const Parameter& p) {
        j = json{
                    {"name", p.name()},
                    {"parameterMode", to_string_parametermode(p.parameterMode())},
                    {"expectType", p.expectType()},
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

    INLINE bool Actual::containsFlag(const std::string& opt) const
    {
        auto tokenClass = (ArgType) classifyToken(opt, app->combineOpts);
        std::string baseOption;
        if (tokenClass == ShortOption) {
            auto it = app->shorthandMap.find(opt);
            if (it != app->shorthandMap.end()) {
                baseOption = it->second;
            } else return false;
        } else
            baseOption = opt;
        return flagSet.find(baseOption) != flagSet.end();
    }

    INLINE void Actual::clearActual() {
        arguments.clear();
        flagSet.clear();
        errNumber = 0;
        errorStr = "";
        mostSimilar.clear();
    }

    INLINE std::optional<std::string> Actual::getValue(const std::string& key) const {
        auto it = parameterMap.find(key);
        if (it != parameterMap.end()) {
            return it->second;
        } else {
            return std::nullopt;
        }
    }

    INLINE void Formal::checkNames(Application *app, const std::string &name, const std::string &shorthand) {
        std::string errStr = tokenError(name, {ArgType::LongOption, ArgType::GccOption, ArgType::ShortOption}, app->combineOpts);
        if (!errStr.empty())
            throw std::invalid_argument(errStr);
        errStr = tokenError(shorthand, {ArgType::ShortOption, ArgError::InvalidEmpty}, app->combineOpts);
        if (!errStr.empty())
            throw std::invalid_argument(errStr);
    }

    INLINE void Formal::addShorthand(Application *app, const std::string &name, std::string shorthand) {
        auto nameClass = classifyToken(name, app->combineOpts);
        if (nameClass == ArgType::ShortOption) {
            if (shorthand.empty())
                shorthand = name;
            else if (shorthand != name)
                throw std::invalid_argument(fmt("two short forms are differ %s and %s",
                    name.c_str(), shorthand.c_str()));
        }
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
        if (isGlobal && app->globalOptionsLocked)
            throw std::logic_error("global options already locked. use this method before AddCommand");
        checkNames(app, name, shorthand);
        addShorthand(app, name, shorthand);
        if (optionMap.find(name) != optionMap.end())
            throw std::invalid_argument(fmt("option '%s' already exists", name.c_str()));
        if (!isGlobal  && app->formal.optionMap.find(name) != app->formal.optionMap.end())
            throw std::invalid_argument(fmt("option '%s' already exists globally", name.c_str()));
        auto flag = std::make_shared<Flag>(name, desc);
        optionMap[name] = flag;
    }

    INLINE void Formal::addParameter(Application *app, const std::string& name, const std::string& shorthand,
        const std::string& desc, const std::string &expect, const std::string &defValue,
        ParameterMode parameterMode)
    {
        if (parameterMode == ParameterMode::Defaulted && defValue.empty())
            throw std::logic_error("default parameter can't be empty");
        if (isGlobal && app->globalOptionsLocked)
            throw std::logic_error("global options already locked. use this method before AddCommand");
        checkNames(app, name, shorthand);
        addShorthand(app, name, shorthand);
        if (optionMap.find(name) != optionMap.end())
            throw std::invalid_argument(fmt("option '%s' already exists", name.c_str()));
        if (!isGlobal  && app->formal.optionMap.find(name) != app->formal.optionMap.end())
            throw std::invalid_argument(fmt("option '%s' already exists globally", name.c_str()));
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
            m_name.c_str(), app->appName.c_str(), app->appName.c_str());
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
        if (errNumber == ErrorCode::MissingHandler) {
            for (const auto& arg : arguments)
                std::cout << arg.value << " = [" << arg.argument.name() << ":" << arg.argument.expectType() << "]\n";
            for (const auto& flag : flagSet)
                std::cout << flag << std::endl;;
            for (const auto& p : parameterMap)
                std::cout << p.first << " : " << p.second << std::endl;;
        }
        else if (errNumber == ErrorCode::UnknownCommand) {
            printSimilars();
        }
    }

    INLINE int Command::execute()
    {
        if (errNumber) {
            printErrors();
            return errNumber;
        } else
            return m_handler(this);
    }

    INLINE void Command::print() const
    {
        Node root(fmt("Command [%s]", m_name.c_str()));
        Node* positional = root.add(Node(fmt("Positional args: (size: %d)",
            formal.argList.size())));
        for (const auto& arg: formal.argList)
        {
            positional->add(Node(arg.name() + " : " + arg.expectType()));
        }
        if (formal.vaArgs.max_n > 0)
        {
            positional = root.add(Node(fmt("VarPositional args: (min: %d, max: %d)",
                        formal.vaArgs.min_n, formal.vaArgs.max_n)));
            positional->add(Node(formal.vaArgs.name() + " : " + formal.vaArgs.expectType()));
        }
        Node* flags = root.add(Node("Flags"));
        for (const auto& arg: flagSet)
        {
            flags->add(Node(arg));
        }
        printTree(root);
    }

    INLINE Command& Command::addParameter(const std::string& name, const std::string& shorthand,
        const std::string& desc, const std::string &expect)
    {
        formal.addParameter(app, name, shorthand, desc, expect, "", ParameterMode::Optional);
        return *this;
    }

    INLINE Command& Command::addReqParameter(const std::string& name, const std::string& shorthand,
        const std::string& desc, const std::string &expect)
    {
        formal.addParameter(app, name, shorthand, desc, expect, "", ParameterMode::Required);
        return *this;
    }

    INLINE Command& Command::addDefParameter(const std::string& name, const std::string& shorthand,
        const std::string& desc, const std::string &expect, const std::string &defValue)
    {
        formal.addParameter(app, name, shorthand, desc, expect, defValue, ParameterMode::Defaulted);
        return *this;
    }

    INLINE Command& Command::overrideParameter(const std::string& name, ParameterMode parameterMode,
        const std::string& defValue)
    {
        if (parameterMode == ParameterMode::Defaulted) {
            if (defValue.empty())
                throw std::logic_error("default parameter can't be empty for mode Defaulted");
        } else {
            if (!defValue.empty())
                throw std::logic_error("default parameter must be empty for mode != Defaulted");
        }
        if (formal.optionMap.find(name) != formal.optionMap.end())
            throw std::invalid_argument(fmt("local option '%s' already exists", name.c_str()));
        auto it = app->formal.optionMap.find(name);
        if (it == app->formal.optionMap.end())
            throw std::invalid_argument(fmt("global option '%s' must exist", name.c_str()));
        auto opt = it->second.get();
        auto base_parameter = dynamic_cast<Parameter*>(opt);
        auto parameter = std::make_shared<Parameter>(*base_parameter, parameterMode, defValue);
        formal.optionMap[name] = parameter;
        return *this;
    }

    INLINE Command& Command::hideOption(const std::string& name) {
        if (formal.optionMap.find(name) != formal.optionMap.end())
            throw std::invalid_argument(fmt("local option '%s' already exists", name.c_str()));
        auto it = app->formal.optionMap.find(name);
        if (it == app->formal.optionMap.end())
            throw std::invalid_argument(fmt("global option '%s' must exist", name.c_str()));
        hiddenOptNames.insert(name);
        return *this;
    }

    INLINE void Command::buildMergedOptions() {
        availableOptionMap = formal.optionMap;
        for (const auto& [key, value] : app->formal.optionMap) {
            if (availableOptionMap.find(key) == availableOptionMap.end())
                availableOptionMap[key] = value;
        }
        for (const auto& name: hiddenOptNames) {
            availableOptionMap.erase(name);
        }
    }

    INLINE std::string Flag::to_string() const
    {
        std::string indent(3, ' ');
        std::string result =  indent + name() + std::string(std::max(1, 10 - static_cast<int>(name().size())), ' ')
                + description();
        return result;
    }

    INLINE Parameter::Parameter(std::string name, std::string description, std::string defVal, std::string expectType,
                ParameterMode parameterMode) : Option(std::move(name), std::move(description)),m_expectType(std::move(expectType)),
                m_parameterMode(parameterMode), m_defValue(std::move(defVal))
    {
        auto& vm = ValidatorManager::instance();
        bool b = vm.testNames(this->m_expectType);
        if (!b)
            throw std::invalid_argument(fmt("expected type '%s' is not registerd", this->m_expectType.c_str()));
        if (m_parameterMode == ParameterMode::Defaulted) {
            std::string found;
            b = vm.validate(m_defValue, m_expectType, found);
            if (!b)
                throw std::invalid_argument(fmt("expected type of default argument = '%s' is not type of '%s'",
                    m_defValue.c_str(), m_expectType.c_str()));
        }
    }

    INLINE std::string Parameter::to_string() const {
        std::string indent(3, ' ');
        std::string result =  indent + name() + std::string(std::max(1, 10 - static_cast<int>(name().size())), ' ')
                + description();
        return result;
    }

    INLINE Argument::Argument(std::string name, std::string expectType): m_name(std::move(name)), m_expectType(std::move(expectType))
    {
        bool b = ValidatorManager::instance().testNames(this->m_expectType);
        if (!b)
            throw std::invalid_argument(fmt("expected type '%s' is not registerd", this->m_expectType.c_str()));
    }

    INLINE std::vector<std::string> Command::preprocessEquals(int start, const std::vector<std::string>& args) const {
        std::vector<std::string> separated;
        for (int i = 0; i < start; i++) {
            separated.push_back(args[i]);
        }
        size_t argNumber = start;
        while (argNumber < args.size()) {
            auto& arg = args[argNumber];
            auto tokenClass = (ArgType) classifyToken(arg, app->combineOpts);
            if (in(tokenClass, {ShortEquals, LongEquals, CompactEquals, GccEquals})) {
                auto p = splitEquals(arg);
                separated.push_back(p.first);
                separated.push_back(p.second);
            } else
                separated.push_back(arg);
            argNumber++;
        }
        return separated;
    }

    /* assume is preprocessEquals first*/
    INLINE std::vector<std::string> Command::preprocessCompact(int start, const std::vector<std::string>& args) const {
        if (!app->combineOpts)
            return args;
        std::vector<std::string> separated;
        for (int i = 0; i < start; i++) {
            separated.push_back(args[i]);
        }
        size_t argNumber = start;
        while (argNumber < args.size()) {
            auto& arg = args[argNumber];
            auto tokenClass = (ArgType) classifyToken(arg, app->combineOpts);
            if (tokenClass == CompactFlags) {
                for (size_t i = 1; i < arg.size(); i++) {
                    std::string newShort = "-";
                    newShort += arg[i];
                    separated.push_back(newShort);
                }
            } else
                separated.push_back(arg);
            argNumber++;
        }
        return separated;
    }

    INLINE void Command::parse(int start, const std::vector<std::string>& args) {
        auto args1 = preprocessEquals(start, args);
        auto args2 = preprocessCompact(start, args1);
        return parsePreprocessed(start, args2);
    }

    /* assume is preprocessEquals and preprocessCompact first*/
    INLINE void Command::parsePreprocessed(int start, const std::vector<std::string>& args)
    {
        clearActual();
        buildMergedOptions();
        size_t count = 0, varCount = 0;
        std::map<std::string, int> optCount;
        for (const auto& [key, _] : availableOptionMap) {
            optCount[key] = 0;
        }
        size_t argNumber = start;
        while (argNumber < args.size())
        {
            auto arg = args[argNumber];
            auto tokenClass = classifyToken(arg, app->combineOpts);
            std::string optStr;
            switch (tokenClass) {
                case LongOption: case GccOption:
                    optStr = arg;
                    break;
                case ShortOption: {
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
                auto it = availableOptionMap.find(optStr);
                if (it  == availableOptionMap.end())
                {
                    errorStr = fmt(ErrorMessage::UnknownLongOption, optStr.c_str());
                    errNumber = ErrorCode::UnknownLongOption;
                    return;
                }
                optCount[it->first]++;
                if (optCount[optStr] > 1) {
                    errorStr = fmt(ErrorMessage::OptionUsedTwice, optStr.c_str());
                    errNumber = ErrorCode::OptionUsedTwice;
                    return;
                }
                auto opt = it->second.get();
                if (opt->kind() == OptionKind::Flag)
                    flagSet.insert(optStr);
                else if (opt->kind() == OptionKind::Parameter) {
                    std::string optArg;
                    auto parameter = dynamic_cast<Parameter*>(opt);
                    argNumber++;
                    if (argNumber >= args.size()) {
                        errorStr = fmt(ErrorMessage::UnexpectedCommandLineEnd, parameter->name().c_str());
                        errNumber = ErrorCode::UnexpectedCommandLineEnd;
                        return;
                    }
                    optArg = args[argNumber];

                    auto& vm = ValidatorManager::instance();
                    std::string found;
                    bool validated = vm.validate(optArg,parameter->expectType(),found);
                    if (!validated) {
                        errorStr = fmt(ErrorMessage::IsNotExpectedTypeParam, optArg.c_str(),
                            parameter->expectType().c_str(), arg.c_str());
                        errNumber = ErrorCode::IsNotExpectedTypeParam;
                        return;
                    }
                    parameterMap[parameter->name()] = optArg;
                }
            }
            else
            {
                Argument formalArgument("string","string");//default value when formal.vaArgs empty
                if (count < formal.argList.size())
                {
                    formalArgument = formal.argList[count++];
                } else if (formal.vaArgs.max_n > 0)
                {
                    formalArgument = *static_cast<Argument*>(&formal.vaArgs);
                    varCount++;
                }
                auto& vm = ValidatorManager::instance();
                std::string found;
                auto expectType = formalArgument.expectType();
                bool validated = vm.validate(arg, expectType,found);
                if (!validated) {
                    errorStr = fmt(ErrorMessage::IsNotExpectedTypeArg, arg.c_str(),
                        formalArgument.expectType().c_str(), formalArgument.name().c_str());
                    errNumber = ErrorCode::IsNotExpectedTypeArg;
                    return;
                }
                arguments.emplace_back(formalArgument, arg);
            }
            argNumber++;
        }
        for (const auto& pair : optCount) {
            if (pair.second == 0) {
                auto option = availableOptionMap[pair.first].get();
                if (option->kind() != OptionKind::Parameter)
                    continue;
                auto parameter = dynamic_cast<Parameter*>(option);
                if (parameter->parameterMode() == ParameterMode::Required) {
                    errorStr = fmt(ErrorMessage::RequiredParameterMissing, parameter->name().c_str());
                    errNumber = ErrorCode::RequiredParameterMissing;
                    return;
                } else if (parameter->parameterMode() == ParameterMode::Defaulted) {
                    parameterMap[parameter->name()] = parameter->defValue();
                }
            }
        }
        if (arguments.size() < formal.argList.size() + formal.vaArgs.min_n)
        {
            errNumber = ErrorCode::TooFewArguments;
            if (arguments.size() > 1)
                errorStr = fmt(ErrorMessage::TooFewArguments,
                    m_name.c_str(), arguments.size(),
                    formal.argList.size() + formal.vaArgs.min_n);
            else
                errorStr = fmt(ErrorMessage::TooFewArguments1,
                    m_name.c_str(),
                    formal.argList.size() + formal.vaArgs.min_n);

        }
        else if (arguments.size() > formal.argList.size() + formal.vaArgs.max_n)
        {
            errNumber = ErrorCode::TooManyArguments;
            if (arguments.size() > 1)
                errorStr = fmt(ErrorMessage::TooManyArguments,
                                m_name.c_str(), arguments.size(),
                                formal.argList.size() + formal.vaArgs.max_n);
            else
                errorStr = fmt(ErrorMessage::TooManyArguments1,
                                m_name.c_str(),
                                formal.argList.size() + formal.vaArgs.max_n);

        }
        else if (!m_handler)
        {
            errNumber = ErrorCode::MissingHandler;
            errorStr = fmt(ErrorMessage::MissingHandler, m_name.c_str());
        }
    }

    INLINE void Command::parseHelpCommand(int start, const std::vector<std::string> &args) {
        clearActual();
        if (app->cmdDepth == 3)
            for (size_t i = start; i < args.size(); i++) {
                if (args[i] == "--all" || args[i] == "-a")
                    flagSet.insert(args[i]);
            }
        for (size_t i = start; i < args.size(); i++) {
            int type = classifyToken(args[i], app->combineOpts);
            if (type == BareIdentifier) {
                Argument formalArgument("command", "identifier");
                arguments.emplace_back(formalArgument, args[i]);
                auto cmd = app->getCommand(args[i]);
                cmd->buildMergedOptions();
                break;
            }
        }
    }

    INLINE Command& Category::addCommand(std::string commandName)
    {
        app->globalOptionsLocked = true;
        auto errStr = tokenError(commandName, ArgType::BareIdentifier, app->combineOpts);
        if (!errStr.empty())
            throw std::invalid_argument(errStr);
        if (commandName == "help")
            throw std::invalid_argument("Command name 'help' is reserved");
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
        auto errStr = tokenError(commandName, ArgType::BareIdentifier, app->combineOpts);
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

    INLINE int Application::execute()
    {
        if (currentCommand)
            return currentCommand->execute();
        else
            return 255;
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
        keys.emplace_back("--help");
        if (helpAvailability > 0)
            keys.emplace_back("help");
        auto mostSimilar = findMostSimilar(arg, keys);

        return mostSimilar;
    }

    INLINE bool Application::findHelpOption(const std::vector<std::string>& args) {
        for (size_t i = 1; i < args.size(); i++)
            if (args[i] == "--help")
                return true;
        return false;
    }

    INLINE void Application::parse(const std::vector<std::string>& args)
    {
        if (helpAvailability > 0 && args.size()>1 && args[1] == "help") {
            currentCommand = helpCommand;
            currentCommand->parseHelpCommand(2, args);
            return;
        }
        if (findHelpOption(args)) {
            currentCommand = helpCommand;
            currentCommand->parseHelpCommand(1, args);
            return;
        }
        if (cmdDepth == 0)
        {
            currentCommand = mainCommand;
            currentCommand->parse(1, args);
        }
        else
        {
            if (args.size() < 2) {
                currentCommand = mainCommand;
                return;
            }
            auto it = commandMap.find(args[1]);
            if (it == commandMap.end())
            {
                currentCommand = std::make_shared<Command>(args[1], this);
                currentCommand->commandNotFound(args[1]);
            }
            else
            {
                currentCommand = it->second;
                currentCommand->parse(2, args);
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

    INLINE int Application::run(int argc, char** argv)
    {
        parse(argc, argv);
        return execute();
    }

    INLINE Command& Application::addCommand(std::string commandName)
    {
        globalOptionsLocked = true;
        auto errStr = tokenError(commandName, ArgType::BareIdentifier, combineOpts);
        if (!errStr.empty())
            throw std::invalid_argument(errStr);
        if (commandName == "help")
            throw std::invalid_argument("Command name 'help' is reserved");
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
        const bool bAll = actual->containsFlag("--all")
                        || actual->containsFlag("-a");

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
        return;
    }

    INLINE int Application::commandHelp(Actual* actual)
    {
        auto arg = actual->arguments[0];
        auto it = commandMap.find(arg.value);
        if (it == commandMap.end())
        {
            auto cmd = std::make_shared<Command>(arg.value, this);
            cmd->commandNotFound(arg.value);
            cmd->printErrors();
            return 0;
        }
        auto cmd = it->second;
        std::cout << cmd->to_string() << std::endl;
        for (const auto& [key, opt] : cmd->availableOptionMap) {
            std::cout << opt->to_string() << std::endl;
        }
        return 0;
    }


    INLINE int Application::help(Actual* actual)
    {
        if (actual->arguments.empty())
            printCommands(actual);
        else
            commandHelp(actual);
        return 0;
    }

    INLINE int Application::helpAboutHelp() const {
        std::cout << appName << ": ";
        std::cout << "use --help";
        if (helpAvailability > 0)
            std::cout << " or help";
        if (cmdDepth == 3)
            std::cout << " [--all]";
        std::cout << " [command]";
        std::cout << std::endl;
        return 0;
    }

    INLINE int Application::mainCommandStub(Actual* actual)
    {
        if (cmdDepth == 0)
        {
            std::cout << "stub command, use:" << std::endl << std::endl;
            std::cout << "  auto mainCommand = app.mainCommand;" << std::endl;
            std::cout << "  mainCommand->setHandler(mainHandler);" << std::endl;
            return 1;
        }
        else if (helpAvailability == 2)
            return help(actual);
        else
            return helpAboutHelp();
    }

    INLINE void Application::initSystemCommands()
    {
        Action actionStub = [this](Actual* actual)->int {
            return mainCommandStub(actual);
        };
        mainCommand = std::make_shared<Command>(appName, this);
        mainCommand->desc(appName);
        mainCommand->handler(actionStub);
        mainCommand->app = this;

        Action actionHelp = [this](Actual* actual)->int {
            return help(actual);
        };
        helpCommand = std::make_shared<Command>("help", this);
        helpCommand->desc("Display help information about " + appName);
        helpCommand->handler(actionHelp);
        helpCommand->app = this;
        if (cmdDepth==3)
            helpCommand->addFlag("--all", "", "all commands");
    }

    INLINE void Application::registerValidators() {
        auto& vm = ValidatorManager::instance();
        vm.register_validator(std::make_unique<IdentifierValidator>());
        vm.register_validator(std::make_unique<UrlValidator>());
        vm.register_validator(std::make_unique<LinuxPathValidator>());
        vm.register_validator(std::make_unique<WindowsPathValidator>());
        vm.register_validator(std::make_unique<GeneralPathValidator>());
        vm.register_validator(std::make_unique<AutoPathValidator>());
        vm.register_validator(std::make_unique<IntegerValidator>());
        vm.register_validator(std::make_unique<DecimalValidator>());
        vm.register_validator(std::make_unique<FloatValidator>());
        vm.register_validator(std::make_unique<NumberValidator>());
    }

    INLINE Application::~Application() {
        ValidatorManager::instance().unregister_all_validators();
    }

    INLINE Application::Application(std::string appName, int cmdDepth_, int combineOpts_, int helpAvailability_):
            formal(true), cmdDepth(cmdDepth_), combineOpts(combineOpts_), helpAvailability(helpAvailability_), appName(std::move(appName))
    {
        if (!appName.empty())
            throw std::invalid_argument("appName is empty");
        registerValidators();
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
    // can be overridden
    INLINE Application& Application::addParameter(const std::string& name, const std::string& shorthand,
        const std::string& desc, const std::string &expect)
    {
        formal.addParameter(this, name, shorthand, desc, expect, "", ParameterMode::Optional);
        return *this;
    }

    INLINE Application& Application::addReqParameter(const std::string& name, const std::string& shorthand,
        const std::string& desc, const std::string &expect)
    {
        formal.addParameter(this, name, shorthand, desc, expect, "", ParameterMode::Required);
        return *this;
    }

    INLINE Application& Application::addDefParameter(const std::string& name, const std::string& shorthand,
        const std::string& desc, const std::string &expect, const std::string &defValue)
    {
        formal.addParameter(this, name, shorthand, desc, expect, defValue, ParameterMode::Defaulted);
        return *this;
    }

    INLINE Application& Application::addFlag(const std::string& name, const std::string& shorthand, const std::string& desc)
    {
        formal.addFlag(this, name, shorthand, desc);
        return *this;
    }

}
