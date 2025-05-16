#pragma once
#include <regex>
#include <string>
#include <unordered_map>
#include <vector>

namespace cli {
    class Validator {
    protected:
        std::regex urlRegex;
        bool validateWith(const std::string& value, const std::string& names, std::string& found) const;
    public:
        virtual ~Validator() = default;
        virtual void init() {
            urlRegex = std::regex(urlRegexStr(), std::regex::icase);
        }
        [[nodiscard]] virtual std::string name() const = 0;
        [[nodiscard]] virtual bool validate(const std::string& value, std::string& found) const;
        [[nodiscard]] virtual std::vector<std::string> dependencies() const { return {}; }
        [[nodiscard]] virtual std::string description() const { return {}; }
        [[nodiscard]] virtual std::string urlRegexStr() const { return "";}
    };

    class ValidatorManager {
        ValidatorManager() = default;
        std::unordered_map<std::string, std::unique_ptr<Validator>> validators_;
    public:
        static ValidatorManager& instance();
        void register_validator(std::unique_ptr<Validator> v);
        void unregister_all_validators();
        bool testName(const std::string &name);
        bool testNames(const std::string &names);
        const Validator& get(const std::string& name) const;
        bool exists(const std::string& name) const;
        bool validate(const std::string& value, const std::string& names, std::string& found) const;
        static bool isNameIdentifier(const std::string& name);
    };

    class IdentifierValidator : public Validator {
        [[nodiscard]] std::string urlRegexStr() const override;
    public:
        [[nodiscard]] std::string name() const override { return "identifier"; }
        [[nodiscard]] std::string description() const override {
            return "only ascii letters or dashes and starts/ends with letter";
        }
    };

    class UrlValidator : public Validator {
        [[nodiscard]] std::string urlRegexStr() const override;
    public:
        [[nodiscard]] std::string name() const override { return "url"; }
        [[nodiscard]] std::string description() const override {
            return "URL starting with http:// or https:// or other schema";
        }
    };

    class LinuxPathValidator : public Validator {
        [[nodiscard]] std::string urlRegexStr() const override;
    public:
        [[nodiscard]] std::string name() const override { return "linux-path"; }
        [[nodiscard]] std::string description() const override {
            return "Absolute Linux path (starts with /)";
        }
    };

    class WindowsPathValidator : public Validator {
        [[nodiscard]] std::string urlRegexStr() const override;
    public:
        [[nodiscard]] std::string name() const override { return "windows-path"; }
        [[nodiscard]] std::string description() const override {
            return "Windows path like C:\\Program Files";
        }
    };

    class AutoPathValidator : public Validator {
    public:
        [[nodiscard]] std::string name() const override { return "auto-path"; }
        [[nodiscard]] bool validate(const std::string& value, std::string& found) const override;
        [[nodiscard]] std::vector<std::string> dependencies() const override {
            return {"linux-path", "windows-path"};//return both to avoid error when paltform changed
        }
        [[nodiscard]] std::string description() const override {
            return "Platform-specific path (Windows or Linux)";
        }
    };

    class GeneralPathValidator : public Validator {
    public:
        [[nodiscard]] std::string name() const override { return "path"; }
        [[nodiscard]] bool validate(const std::string& value, std::string& found) const override;
        [[nodiscard]] std::vector<std::string> dependencies() const override {
            return {"linux-path", "windows-path"};
        }
        [[nodiscard]] std::string description() const override {
            return "One path (Windows or Linux) from both";
        }
    };

    class IntegerValidator : public Validator {
        [[nodiscard]] std::string urlRegexStr() const override;
    public:
        [[nodiscard]] std::string name() const override { return "integer"; }
        [[nodiscard]] std::string description() const override {
            return "integer numbers, can be <0, not starting with 0 except 0";
        }
    };

    class DecimalValidator : public Validator {
        [[nodiscard]] std::string urlRegexStr() const override;
    public:
        [[nodiscard]] std::string name() const override { return "decimal"; }
        [[nodiscard]] std::string description() const override {
            return "float numbers without exponent like 123.456";
        }
    };


    class FloatValidator : public Validator {
        [[nodiscard]] std::string urlRegexStr() const override;
    public:
        [[nodiscard]] std::string name() const override { return "float"; }
        [[nodiscard]] std::string description() const override {
            return "float numbers without and with exponent like 1.23e12";
        }
    };

    class NumberValidator : public Validator {
    public:
        [[nodiscard]] std::string name() const override { return "number"; }
        [[nodiscard]] bool validate(const std::string& value, std::string& found) const override;
        [[nodiscard]] std::vector<std::string> dependencies() const override {
            return {"integer", "decimal","float"};
        }
        [[nodiscard]] std::string description() const override {
            return "Any number, integer or not";
        }
    };
}
