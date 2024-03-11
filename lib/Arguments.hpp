#pragma once

#include <cinttypes>
#include <iostream>
#include <limits>
#include <map>
#include <string>
#include <vector>

namespace ArgumentParser {

// Base argument
class BaseArgument {
   public:
    virtual ~BaseArgument() = default;
    BaseArgument() = default;
    BaseArgument(char short_name, const std::string& name,
                 const std::string& description)
        : short_name_(short_name), name_(name), description_(description) {}

    std::string name() const { return name_; }
    std::string description() const { return description_; }
    char short_name() const { return short_name_; }

    virtual void SetValue(const std::string& value = "") = 0;
    virtual bool IsCorrect() const = 0;
    virtual bool IsPositional() const { return false; }
    virtual bool IsMultiValue() const { return false; }
    virtual int32_t ValuesCount() const { return 1; }
    virtual std::string GetDefaultValue() const { return ""; }

   private:
    std::string name_ = "";
    std::string description_ = "";
    char short_name_ = '\0';
};

// Int argument
class IntArgument : public BaseArgument {
   public:
    IntArgument() = default;
    IntArgument(char short_name, const std::string& name,
                const std::string& description)
        : BaseArgument(short_name, name, description) {}
    ~IntArgument() {
        if (is_stored_) {
            return;
        }
        delete value_;
        delete multi_value_;
    }

    void SetValue(const std::string& value) override {
        if (is_multi_value_) {
            if (multi_value_ == nullptr) {
                multi_value_ = new std::vector<int32_t>();
            }
            multi_value_->push_back(std::stoi(value));
        } else {
            if (value_ == nullptr) {
                value_ = new int32_t();
            }
            *value_ = std::stoi(value);
        }
        is_set_ = true;
    }

    bool IsCorrect() const override {
        if (is_multi_value_) {
            return !(multi_value_->size() < multi_value_count_ &&
                     multi_value_count_ != 0);
        }
        return is_set_ || is_default_;
    }

    bool IsPositional() const override { return is_positional_; }

    bool IsMultiValue() const override { return is_multi_value_; }

    int32_t ValuesCount() const override {
        if (is_multi_value_) {
            if (multi_value_count_ != 0) {
                return multi_value_count_;
            }
            return std::numeric_limits<int32_t>::max();
        }
        return 1;
    }

    std::string GetDefaultValue() const override {
        if (is_default_) {
            return std::to_string(default_value_);
        }
        return "";
    }

    int32_t GetValue(int32_t index = 0) const {
        if (is_multi_value_) {
            if (is_set_) {
                return multi_value_->at(index);
            }
            return default_multi_value_.at(index);
        } else {
            if (is_set_) {
                return *value_;
            }
            return default_value_;
        }
    }

    IntArgument& Positional() {
        is_positional_ = true;
        return *this;
    }

    IntArgument& Default(int32_t value) {
        if (is_multi_value_) {
            throw std::runtime_error(
                "Default value for multi value argument is not supported");
        }
        default_value_ = value;
        is_default_ = true;
        return *this;
    }

    IntArgument& Default(const std::vector<int32_t>& values) {
        if (!is_multi_value_) {
            throw std::runtime_error(
                "Default value for single value argument is not supported");
        }
        default_multi_value_ = values;
        is_default_ = true;
        return *this;
    }

    IntArgument& MultiValue(int32_t count = 0) {
        multi_value_count_ = count;
        is_multi_value_ = true;
        return *this;
    }

    IntArgument& StoreValue(int32_t& value) {
        if (is_multi_value_) {
            throw std::runtime_error(
                "Store value for multi value argument is not supported");
        }
        value_ = &value;
        is_stored_ = true;
        return *this;
    }

    IntArgument& StoreValues(std::vector<int32_t>& values) {
        if (!is_multi_value_) {
            throw std::runtime_error(
                "Store value for single value argument is not supported");
        }
        multi_value_ = &values;
        is_stored_ = true;
        return *this;
    }

    bool is_default() const { return is_default_; }

   private:
    int32_t* value_ = nullptr;
    std::vector<int32_t>* multi_value_ = nullptr;
    int32_t default_value_;
    std::vector<int32_t> default_multi_value_;

    bool is_default_ = false;
    int32_t multi_value_count_ = 0;
    bool is_multi_value_ = false;
    bool is_set_ = false;
    bool is_positional_ = false;
    bool is_stored_ = false;
};

// String argument
class StringArgument : public BaseArgument {
   public:
    StringArgument() = default;
    StringArgument(char short_name, const std::string& name,
                   const std::string& description)
        : BaseArgument(short_name, name, description) {}
    ~StringArgument() {
        if (is_stored_) {
            return;
        }
        delete value_;
        delete multi_value_;
    }

    void SetValue(const std::string& value) override {
        if (is_multi_value_) {
            if (multi_value_ == nullptr) {
                multi_value_ = new std::vector<std::string>();
            }
            multi_value_->push_back(value);
        } else {
            if (value_ == nullptr) {
                value_ = new std::string();
            }
            *value_ = value;
        }
        is_set_ = true;
    }

    bool IsCorrect() const override {
        if (is_multi_value_) {
            return !(multi_value_->size() < multi_value_count_ &&
                     multi_value_count_ != 0);
        }
        return is_set_ || is_default_;
    }

    bool IsPositional() const override { return is_positional_; }

    bool IsMultiValue() const override { return is_multi_value_; }

    int32_t ValuesCount() const override {
        if (is_multi_value_) {
            if (multi_value_count_ != 0) {
                return multi_value_count_;
            }
            return std::numeric_limits<int32_t>::max();
        }
        return 1;
    }

    std::string GetDefaultValue() const override { 
        if (is_default_) {
            return default_value_;
        }
        return "";
    }

    std::string GetValue(int32_t index = 0) const {
        if (is_multi_value_) {
            if (is_set_) {
                return multi_value_->at(index);
            }
            return default_multi_value_.at(index);
        } else {
            if (is_set_) {
                return *value_;
            }
            return default_value_;
        }
    }

    StringArgument& Positional() {
        is_positional_ = true;
        return *this;
    }

    StringArgument& Default(const std::string& value) {
        if (is_multi_value_) {
            throw std::runtime_error(
                "Default value for multi value argument is not supported");
        }
        default_value_ = value;
        is_default_ = true;
        return *this;
    }

    StringArgument& Default(const std::vector<std::string>& values) {
        if (!is_multi_value_) {
            throw std::runtime_error(
                "Default value for single value argument is not supported");
        }
        default_multi_value_ = values;
        is_default_ = true;
        return *this;
    }

    StringArgument& MultiValue(int32_t count = 0) {
        multi_value_count_ = count;
        is_multi_value_ = true;
        return *this;
    }

    StringArgument& StoreValue(std::string& value) {
        if (is_multi_value_) {
            throw std::runtime_error(
                "Store value for multi value argument is not supported");
        }
        value_ = &value;
        is_stored_ = true;
        return *this;
    }

    StringArgument& StoreValues(std::vector<std::string>& values) {
        if (!is_multi_value_) {
            throw std::runtime_error(
                "Store value for single value argument is not supported");
        }
        multi_value_ = &values;
        is_stored_ = true;
        return *this;
    }

    bool is_default() const { return is_default_; }

   public:
    std::string* value_ = nullptr;
    std::vector<std::string>* multi_value_ = nullptr;
    std::string default_value_;
    std::vector<std::string> default_multi_value_;

    bool is_default_ = false;
    int32_t multi_value_count_ = 0;
    bool is_multi_value_ = false;
    bool is_set_ = false;
    bool is_positional_ = false;
    bool is_stored_ = false;
};

// Flag argument
class FlagArgument : public BaseArgument {
   public:
    FlagArgument() = default;
    FlagArgument(char short_name, const std::string& name,
                 const std::string& description)
        : BaseArgument(short_name, name, description) {}
    ~FlagArgument() {
        if (is_stored_) {
            return;
        }
        delete value_;
    }

    void SetValue(const std::string& value) override {
        if (value_ == nullptr) {
            value_ = new bool();
        }
        *value_ = !default_value_;
    }

    bool IsCorrect() const override { return true; }

    std::string GetDefaultValue() const override {
        return default_value_ ? "true" : "false";
    }

    bool GetValue(int32_t index = 0) const {
        if (value_ == nullptr) {
            return default_value_;
        }
        return *value_;
    }

    FlagArgument& Default(bool value) {
        default_value_ = value;
        return *this;
    }

    FlagArgument& StoreValue(bool& value) {
        value_ = &value;
        is_stored_ = true;
        return *this;
    }

    bool IsPositional() const override { return false; }

    int32_t ValuesCount() const override { return 0; }

   private:
    bool* value_ = nullptr;
    bool default_value_ = false;

    bool is_stored_ = false;
};

}  // namespace ArgumentParser