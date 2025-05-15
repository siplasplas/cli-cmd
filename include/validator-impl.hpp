#pragma once
#include <cassert>
#include <set>

#include "validator.h"

namespace cli {
    INLINE ValidatorManager & ValidatorManager::instance() {
        static ValidatorManager instance;
        return instance;
    }

    INLINE bool Validator::validate(const std::string &value, std::string &found) const {
        found = "";
        if (std::regex_match(value.begin(), value.end(), urlRegex)) {
            found = name();
            return true;
        } else
            return false;
    }

    INLINE std::vector<std::string> split_by_space(const std::string& input) {
        std::istringstream stream(input);
        std::vector<std::string> tokens;
        std::string word;
        while (stream >> word) {
            tokens.push_back(word);
        }
        return tokens;
    }

    INLINE bool Validator::validateWith(const std::string &value, const std::string &names, std::string &found) const {
        auto namesList = split_by_space(names);
        found = "";
        if (namesList.empty())
            return false;
        auto depVec = dependencies();
        std::set<std::string> depSet(depVec.begin(), depVec.end());
        for (const auto& n: namesList)
            if (depSet.find(n) == depSet.end()) {
                std::string msg = "Validator '" + n  + "' is not in depedencies of '" + name() + "'";
                throw std::invalid_argument(msg);
            }
        return ValidatorManager::instance().validate(value, names, found);
    }

    INLINE void ValidatorManager::register_validator(std::unique_ptr<Validator> v) {
        const std::string& n = v->name();
        if (n.empty())
            throw std::runtime_error("validator name can't be emopty");
        if (n == "string")
            throw std::runtime_error("'string' is reservwed as validator name");
        if (!isNameIdentifier(n))
            throw std::runtime_error("validator name '"+ n+ "' can be only ascii letters or dashes and starts/ends with letter");
        if (validators_.find(n) != validators_.end())
            throw std::runtime_error("Validator '" + n + "' already registered");

        for (const std::string& dep : v->dependencies()) {
            if (validators_.find(dep) == validators_.end()) {
                throw std::runtime_error("Validator '" + n + "' depends on missing validator '" + dep + "'");
            }
        }
        v->init();
        validators_[n] = std::move(v);
    }

    INLINE void ValidatorManager::unregister_all_validators() {
        validators_.clear();
    }

    INLINE bool ValidatorManager::testName(const std::string &name) {
        if (name.empty())
            return false;
        if (name == "string")
            return true;
        return validators_.find(name) != validators_.end();
    }

    INLINE bool ValidatorManager::testNames(const std::string &names) {
        auto namesList = split_by_space(names);
        if (namesList.empty())
            return false;
        return std::all_of(namesList.begin(), namesList.end(),
                           [this](const auto& name) { return testName(name); });
    }

    INLINE const Validator& ValidatorManager::get(const std::string &name) const{
        auto it = validators_.find(name);
        if (it == validators_.end()) {
            throw std::runtime_error("Validator not found: " + name);
        }
        return *it->second;
    }

    INLINE bool ValidatorManager::exists(const std::string &name) const {
        return validators_.count(name) > 0;
    }

    INLINE bool ValidatorManager::validate(const std::string &value, const std::string &names, std::string& found) const {
        found = "";
        auto namesList = split_by_space(names);
        for (const auto& name: namesList) {
            if (name == "string") {
                found = "string";
                return true;
            }
            auto& validator = get(name);
            if (validator.validate(value, found))
                return true;
        }
        return false;
    }

    INLINE bool ValidatorManager::isNameIdentifier(const std::string &name) {
        assert(!name.empty());
        if (name[0] == '-' || name.back() == '-')
            return false;
        for (char c : name) {
            if (!(isalpha(c) || c == '-')) return false;
        }
        return true;
    }

    INLINE std::string IdentifierValidator::urlRegexStr() const {
        return R"([a-z]([a-z-]*[a-z])?)";
    }

    INLINE std::string UrlValidator::urlRegexStr() const{
        std::string label = R"([a-z0-9]([a-z0-9-]*[a-z0-9])?)";
        std::string labelDot  = label + R"(\.)";
        std::string repeated = "(" + labelDot + ")+";
        std::string domain = repeated + "[a-z]+";
        std::string localhost = "localhost";
        std::string ipv4 = R"((25[0-5]|2[0-4]\d|1\d{2}|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d{2}|[1-9]?\d)){3})";
        std::string hexBlock = R"([0-9a-fA-F]{4,4})";
        std::string ipv6 = R"(\[()" + hexBlock + R"(:){7,7})" + hexBlock +  R"(\])";
        std::string hexBlock_or_ipv4 = hexBlock + "|" + ipv4;
        std::string shortBlock = R"([0-9a-fA-F]{1,4})";
        std::string shortBlock_or_ipv4 = shortBlock + "|" + ipv4;
        std::string blockPlus = R"((()" + hexBlock + R"(:)*)" + hexBlock +  R"()?)";
        std::string blockPlusExt = R"(((()" + shortBlock_or_ipv4 + R"():)*()" + shortBlock_or_ipv4 +  R"())?)";
        std::string ipv6ext = R"(\[)" + blockPlusExt + "::" + blockPlusExt + R"(\])";
        std::string host = domain + "|" + localhost + "|" + ipv4 + "|" + ipv6ext;
        std::string userinfo = "[a-z0-9._~!$&'()*+,;=:%-]+";
        std::string port = "[1-9][0-9]{0,4}";
        std::string authority = "(" + userinfo + "@)?(" + host + ")(:" + port + ")?";
        std::string scheme = "[a-z][a-z0-9+.-]*";
        std::string segment = "[a-z0-9._~!$&'()*+,;=:@%-]*";
        std::string path = "(/" + segment + ")*";
        std::string pchar = R"((([a-z0-9._~!$&'()*+,;=:@/?-]|%[0-9a-fA-F]{2}))*)";
        std::string query = "\\?" + pchar;
        std::string fragment = "#" + pchar;
        std::string  url = scheme + "://" + authority + "(:" + port + ")?" + path + "(" + query + ")?" + "(" + fragment + ")?";
        return url;
    }

    INLINE std::string LinuxPathValidator::urlRegexStr() const {
        std::string res;
        res += "(?:~)?"; // optional ~
        res += "(?:/)?"; // optional /
        res += "(?:[^/\\0]+(?:/[^/\\0]+)*)?";  // segments
        res += "/?";        // terminal optional /
        return res;
    }

    INLINE std::string WindowsPathValidator::urlRegexStr() const {
        std::string drive = R"([a-z]:\\)"; //drive
        std::string res;
        res = "(" + drive + ")?";
        std::string segment = R"([^\\/:*?"<>|\r\n]+)"; //segment without reserved chars
        res += "(" + segment + R"(\\)*)";
        res += segment + "(\\\\)?";
        return res;
    }


    INLINE bool AutoPathValidator::validate(const std::string& value, std::string& found) const {
        std::string subValidator;
    #ifdef _WIN32
        subValidator = "windows-path";
    #else
        subValidator = "linux-path";
    #endif
        return validateWith(value, subValidator, found);
    }

    INLINE bool GeneralPathValidator::validate(const std::string& value, std::string& found) const {
        return validateWith(value, "linux-path windows-path", found);
    }

    INLINE std::string IntegerValidator::urlRegexStr() const {
        return "(0|-?[1-9][0-9]*)";
    }

    INLINE std::string DecimalValidator::urlRegexStr() const {
        return R"([-+]?(?:0|[1-9][0-9]*)(?:\.[0-9]+)?$|^[-+]?(?:0|[1-9][0-9]*)\.)";
    }

    INLINE std::string FloatValidator::urlRegexStr() const {
        return R"(^[-+]?(?:0|[1-9][0-9]*)(?:\.[0-9]+)?(?:[eE][-+]?[0-9]+)?$|^[-+]?(?:0|[1-9][0-9]*)\.(?:[eE][-+]?[0-9]+)?$)";
    }

    INLINE bool NumberValidator::validate(const std::string &value, std::string &found) const {
        return validateWith(value, "integer decimal float", found);
    }
}
