#pragma once

#include <cinttypes>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "Arguments.hpp"

namespace ArgumentParser {

class ArgParser {
   public:
    ArgParser(const std::string& name) : name_(name) {}

    ~ArgParser() {
        for (auto& [name, argument] : arguments_) {
            delete argument;
        }
    }

    // AddHelp
    FlagArgument& AddHelp(char short_name, const std::string& name,
                         const std::string& description);

    // AddStringArgument
    StringArgument& AddStringArgument(char short_name, const std::string& name,
                                     const std::string& description = "");
    StringArgument& AddStringArgument(const std::string& name,
                                     const std::string& description = "");

    // AddIntArgument
    IntArgument& AddIntArgument(char short_name, const std::string& name,
                               const std::string& description = "");
    IntArgument& AddIntArgument(const std::string& name,
                               const std::string& description = "");

    // AddFlag
    FlagArgument& AddFlag(char short_name, const std::string& name,
                         const std::string& description = "");
    FlagArgument& AddFlag(const std::string& name,
                         const std::string& description = "");

    // Get
    std::string GetStringValue(const std::string& name, int32_t index = 0);
    int32_t GetIntValue(const std::string& name, int32_t index = 0);
    bool GetFlag(const std::string& name, int32_t index = 0);

    // Parse
    bool Parse(int32_t argc, char** argv);
    bool Parse(const std::vector<std::string>& args, int32_t index = 1);
    bool UpdatePositionalArguments();
    BaseArgument* GetArgument(const std::string& name) const;

    // Help
    std::string HelpDescription() const;
    bool Help() const;

   private:
    std::string name_ = "";

    std::vector<std::pair<std::string, BaseArgument*>> arguments_;
    std::map<char, std::string> short_names_;

    std::vector<std::pair<int32_t, std::string>> positional_arguments_;
};

}  // namespace ArgumentParser
