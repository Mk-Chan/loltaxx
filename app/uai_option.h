#ifndef LOLTAXX_UAIOPTION_H
#define LOLTAXX_UAIOPTION_H

#include <functional>
#include <unordered_map>
#include <unordered_set>

namespace loltaxx {

template <class T>
class UAIOption {
   public:
    using value_type = T;

    UAIOption() = default;
    UAIOption(std::string name, value_type value, std::function<void(const T&)> handler)
        : name_(std::move(name)), value_(std::move(value)), handler_(std::move(handler)) {
    }

    [[nodiscard]] std::string name() const noexcept {
        return name_;
    }
    [[nodiscard]] value_type value() const noexcept {
        return value_;
    };

    void handler(std::optional<value_type> value) const noexcept {
        return handler_(value);
    }

    virtual void set_option(const value_type& value) noexcept {
        value_ = value;
        handler_(value);
    };

   protected:
    std::string name_;
    value_type value_{};
    std::function<void(const T&)> handler_;
};

template <>
class UAIOption<void> {
   public:
    using value_type = void;

    UAIOption() = default;
    UAIOption(std::string name, std::function<void(void)> handler) noexcept
        : name_(std::move(name)), handler_(std::move(handler)) {
    }

    [[nodiscard]] const std::string& name() const noexcept {
        return name_;
    }
    void handler() const noexcept {
        handler_();
    }

   protected:
    std::string name_;
    std::function<void(void)> handler_;
};

class UAISpinOption : public UAIOption<int> {
   public:
    UAISpinOption() : min_value_(0), max_value_(0) {
    }
    UAISpinOption(const std::string& name,
                  const value_type& value,
                  const value_type& min_value,
                  const value_type& max_value,
                  const std::function<void(const value_type&)>& handler) noexcept
        : UAIOption<value_type>(name, value, handler),
          min_value_(min_value),
          max_value_(max_value) {
    }

    [[nodiscard]] value_type min_value() const noexcept {
        return min_value_;
    }
    [[nodiscard]] value_type max_value() const noexcept {
        return max_value_;
    }
    void set_option(const value_type& value) noexcept override {
        if (value >= min_value() && value <= max_value()) {
            value_ = value;
            handler_(value);
        }
    };

   private:
    value_type min_value_;
    value_type max_value_;
};

class UAIComboOption : public UAIOption<std::string> {
   public:
    UAIComboOption() : UAIOption<std::string>() {
    }
    UAIComboOption(const std::string& name,
                   const value_type& value,
                   std::unordered_set<value_type> allowed_values,
                   const std::function<void(const value_type&)>& handler) noexcept
        : UAIOption<value_type>(name, value, handler), allowed_values_(std::move(allowed_values)) {
    }

    [[nodiscard]] bool is_allowed(const value_type& value) const noexcept {
        return allowed_values_.find(value) != allowed_values_.end();
    }
    const std::unordered_set<value_type>& allowed_values() const noexcept {
        return allowed_values_;
    }
    void set_option(const value_type& value) noexcept override {
        if (is_allowed(value)) {
            UAIOption<value_type>::set_option(value);
        }
    }

   private:
    std::unordered_set<value_type> allowed_values_;
};

class UAIStringOption : public UAIOption<std::string> {
   public:
    UAIStringOption() : UAIOption<std::string>() {
    }
    UAIStringOption(const std::string& name,
                    const value_type& value,
                    const std::function<void(const value_type&)>& handler)
        : UAIOption<value_type>(name, value, handler) {
    }
};

class UAICheckOption : public UAIOption<bool> {
   public:
    UAICheckOption() : UAIOption<bool>() {
    }
    UAICheckOption(const std::string& name,
                   const value_type& value,
                   const std::function<void(const value_type&)>& handler)
        : UAIOption<value_type>(name, value, handler) {
    }
};

class UAIButtonOption : public UAIOption<void> {
   public:
    UAIButtonOption() : UAIOption<void>() {
    }
    UAIButtonOption(const std::string& name, const std::function<void(void)>& handler) noexcept
        : UAIOption<value_type>(name, handler) {
    }
};

}  // namespace loltaxx

#endif  // LOLTAXX_UAIOPTION_H
