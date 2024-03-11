#include "ArgParser.h"

using namespace ArgumentParser;

bool ArgParser::Parse(int32_t argc, char** argv) {
    std::vector<std::string> args = std::vector<std::string>(argv, argv + argc);
    return Parse(args);
}

bool ArgParser::Parse(const std::vector<std::string>& args, int32_t index) {
    if (index >= args.size()) {
        BaseArgument* help = GetArgument("help");
        if (help != nullptr) {
            FlagArgument* flag_argument =
                dynamic_cast<FlagArgument*>(help);
            if (flag_argument != nullptr) {
                return flag_argument->GetValue();
            }
        }

        bool is_positional_correct = UpdatePositionalArguments();
        positional_arguments_.clear();
        if (!is_positional_correct) {
            std::cerr << "Positional arguments are not correct" << std::endl;
            return false;
        }

        for (auto& argument : arguments_) {
            if (!argument.second->IsCorrect()) {
                std::cerr << "Argument " << argument.first << " is not correct"
                          << std::endl;
                return false;
            }
        }
        return true;
    }

    if (args[index][0] == '-' && args[index][1] == '-') {
        std::string name = args[index].substr(2);
        size_t k = name.find('=');
        if (k != std::string::npos) {
            std::string value = name.substr(k + 1);
            name = name.substr(0, k);
            BaseArgument* argument = GetArgument(name);
            if (argument == nullptr) {
                std::cerr << "Unknown argument " << name << std::endl;
                return false;
            }
            argument->SetValue(value);
            return Parse(args, index + 1);
        }
        BaseArgument* argument = GetArgument(name);
        if (argument == nullptr) {
            std::cerr << "Unknown argument " << name << std::endl;
            return false;
        }
        int32_t count = argument->ValuesCount();
        if (count == 0) {
            argument->SetValue();
        }
        while (index + 1 < args.size() && args[index + 1][0] != '-' &&
               count > 0) {
            argument->SetValue(args[index + 1]);
            ++index;
            --count;
        }
        return Parse(args, index + 1);
    }

    if (args[index][0] == '-') {
        std::string names = args[index].substr(1);
        size_t k = names.find('=');
        if (k != std::string::npos) {
            std::string value = names.substr(k + 1);
            names = names.substr(0, k);
            for (auto& sname : names) {
                auto name = short_names_.find(sname);
                if (name == short_names_.end()) {
                    std::cerr << "Unknown argument " << sname << std::endl;
                    return false;
                }
                BaseArgument* argument = GetArgument(name->second);
                if (argument == nullptr) {
                    std::cerr << "Unknown argument " << name->second
                              << std::endl;
                    return false;
                }
                argument->SetValue(value);
            }
            return Parse(args, index + 1);
        }
        for (auto& sname : names) {
            auto name = short_names_.find(sname);
            if (name == short_names_.end()) {
                std::cerr << "Unknown argument " << sname << std::endl;
                return false;
            }
            BaseArgument* argument = GetArgument(name->second);
            if (argument == nullptr) {
                std::cerr << "Unknown argument " << name->second << std::endl;
                return false;
            }
            int32_t count = argument->ValuesCount();
            if (count == 0) {
                argument->SetValue();
            }
            while (index + 1 < args.size() && args[index + 1][0] != '-' &&
                   count > 0) {
                argument->SetValue(args[index + 1]);
                ++index;
                --count;
            }
        }
        return Parse(args, index + 1);
    }

    int32_t number = index;
    while (index < args.size() && args[index][0] != '-') {
        positional_arguments_.push_back(std::make_pair(number, args[index]));
        ++index;
    }

    return Parse(args, index);
}

bool ArgParser::UpdatePositionalArguments() {
    if (positional_arguments_.empty()) {
        return true;
    }
    int32_t index = 0;
    for (auto& argument : arguments_) {
        if (argument.second->IsPositional()) {
            int32_t current = positional_arguments_[0].first;
            while (index < positional_arguments_.size() &&
                   positional_arguments_[index].first == current) {
                argument.second->SetValue(positional_arguments_[index].second);
                ++index;
            }
        }
    }
    return index == positional_arguments_.size();
}

BaseArgument* ArgParser::GetArgument(const std::string& name) const {
    for (auto& argument : arguments_) {
        if (argument.first == name) {
            return argument.second;
        }
    }
    return nullptr;
}

FlagArgument& ArgParser::AddHelp(char short_name, const std::string& name,
                                 const std::string& description) {
    FlagArgument* argument = new FlagArgument(short_name, name, description);
    arguments_.push_back(std::make_pair(name, argument));
    short_names_.insert(std::make_pair(short_name, name));
    return *argument;
}

StringArgument& ArgParser::AddStringArgument(char short_name,
                                             const std::string& name,
                                             const std::string& description) {
    StringArgument* argument =
        new StringArgument(short_name, name, description);
    arguments_.push_back(std::make_pair(name, argument));
    short_names_.insert(std::make_pair(short_name, name));
    return *argument;
}
StringArgument& ArgParser::AddStringArgument(const std::string& name,
                                             const std::string& description) {
    return AddStringArgument('\0', name, description);
}

IntArgument& ArgParser::AddIntArgument(char short_name, const std::string& name,
                                       const std::string& description) {
    IntArgument* argument = new IntArgument(short_name, name, description);
    arguments_.push_back(std::make_pair(name, argument));
    short_names_.insert(std::make_pair(short_name, name));
    return *argument;
}
IntArgument& ArgParser::AddIntArgument(const std::string& name,
                                       const std::string& description) {
    return AddIntArgument('\0', name, description);
}

FlagArgument& ArgParser::AddFlag(char short_name, const std::string& name,
                                 const std::string& description) {
    FlagArgument* argument = new FlagArgument(short_name, name, description);
    arguments_.push_back(std::make_pair(name, argument));
    short_names_.insert(std::make_pair(short_name, name));
    return *argument;
}
FlagArgument& ArgParser::AddFlag(const std::string& name,
                                 const std::string& description) {
    return AddFlag('\0', name, description);
}

std::string ArgParser::GetStringValue(const std::string& name, int32_t index) {
    BaseArgument* argument = GetArgument(name);
    if (argument == nullptr) {
        return "";
    }
    StringArgument* string_argument =
        dynamic_cast<StringArgument*>(argument);
    if (string_argument == nullptr) {
        return "";
    }
    return string_argument->GetValue(index);
}

int32_t ArgParser::GetIntValue(const std::string& name, int32_t index) {
    BaseArgument* argument = GetArgument(name);
    if (argument == nullptr) {
        return 0;
    }
    IntArgument* int_argument = dynamic_cast<IntArgument*>(argument);
    if (int_argument == nullptr) {
        return 0;
    }
    return int_argument->GetValue(index);
}

bool ArgParser::GetFlag(const std::string& name, int32_t index) {
    BaseArgument* argument = GetArgument(name);
    if (argument == nullptr) {
        return false;
    }
    FlagArgument* flag_argument = dynamic_cast<FlagArgument*>(argument);
    if (flag_argument == nullptr) {
        return false;
    }
    return flag_argument->GetValue(index);
}

bool ArgParser::Help() const {
    BaseArgument* help = GetArgument("help");
    if (help == nullptr) {
        return false;
    }
    FlagArgument* flag_argument = dynamic_cast<FlagArgument*>(help);
    if (flag_argument == nullptr) {
        return false;
    }
    return flag_argument->GetValue();
}

std::string ArgParser::HelpDescription() const {
    std::string description = name_ + "\n";
    BaseArgument* help = GetArgument("help");
    if (help != nullptr) {
        description += help->description() + "\n";
    }
    description += "Options:\n";
    for (auto& [name, argument] : arguments_) {
        if (name == "help") {
            continue;
        }

        if (argument->short_name() != '\0') {
            description += "-" + std::string(1, argument->short_name()) + ", ";
        }
        description += "--" + argument->name();
        if (argument->description() != "") {
            description += ", " + argument->description();
        }

        if (argument->IsPositional()) {
            description += ", (positional)";
        }
        if (argument->IsMultiValue()) {
            description += ", (minimum " +
                           std::to_string(argument->ValuesCount()) + " args)";
        }
        if(argument->GetDefaultValue() != "") {
            description += ", (default " + argument->GetDefaultValue() + ")";
        }

        description += "\n";
    }
    return description;
}