#ifndef VALUE_HPP
#define VALUE_HPP

#include "token.hpp"
#include <cctype>
#include <cstddef>
#include <format>
#include <functional>
#include <memory>
#include <source_location>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <variant>

namespace SLang {

enum class ValueType {
  NONE,
  Any,
  INT,
  FLOAT,
  BOOL,
  STRING,
  REFERENCE,
};

class Value {
private:
  using RefType = std::shared_ptr<Value>;
  struct AnyType {};
  using Storage = std::variant<std::monostate, AnyType, int32_t, float,
                               std::string, bool, RefType>;

  Storage data;
  ValueType type;

  auto get_number_type(std::string value) -> ValueType;
  auto string_to_int(std::string value) -> int32_t;
  auto string_to_float(std::string value) -> float;

  auto get_base(std::string value) -> NumberNotation;
  // Return string without number notation
  auto get_number_without_notation(std::string value) -> std::string;

  auto storage_to_value_type(Storage &store) -> ValueType;

  template <typename T> static Storage get_raw_storage(T value) {
    if constexpr (std::is_same_v<T, Storage>) {
      if (std::holds_alternative<RefType>(value)) {
        return get_raw_storage(std::get<RefType>(value)->data);
      } else {
        return value;
      }
    } else if constexpr (std::is_same_v<T, RefType>) {
      return get_raw_storage(value->data);
    } else if constexpr (std::is_same_v<T, Value>) {
      return get_raw_storage(value.data);
    } else {
      return value;
    }
  }

  static RefType get_last_ref(const RefType &value) {
    if (std::holds_alternative<RefType>(value->data)) {
      return get_last_ref(std::get<RefType>(value->data));
    }

    return value;
  }

public:
  Value() : data(std::monostate()), type(ValueType::NONE) {};
  Value(ValueType type);
  Value(Token token);
  Value(int32_t value) : data(value), type(ValueType::INT) {}
  Value(float value) : data(value), type(ValueType::FLOAT) {}
  Value(bool value) : data(value), type(ValueType::BOOL) {}
  Value(std::string value) : data(value), type(ValueType::STRING) {}
  Value(RefType value) : data(value), type(ValueType::REFERENCE) {}
  Value(Storage store) : data(store), type(storage_to_value_type(store)) {}

  [[nodiscard]]
  auto get_type() -> ValueType;

  [[nodiscard]]
  auto get_data() -> Storage &;

  [[nodiscard]]
  static auto type_to_string(ValueType type) -> std::string;

  [[nodiscard]]
  auto type_to_string() -> std::string;

  [[nodiscard]]
  static auto from_token_type(TokenType type) -> ValueType;

  [[nodiscard]]
  auto get_raw_value() -> Value;

  template <typename T, typename F>
  friend T make_operation(
      F op, const Value &lhs, const Value &rhs,
      const std::source_location &loc = std::source_location::current()) {
    auto l = get_raw_storage(lhs);
    auto r = get_raw_storage(rhs);

    return std::visit(
        [&loc, &op](auto &lhs, auto &rhs) -> T {
          if constexpr (requires { op(lhs, rhs); }) {
            return op(lhs, rhs);
          } else {
            throw std::runtime_error(std::format(
                "Cannot apply operation: {} for lhs: {} and rhs: {}",
                loc.function_name(), typeid(lhs).name(), typeid(rhs).name()));
          }
        },
        l, r);
  }

  friend Value operator+(const Value &lhs, const Value &rhs) {
    auto l = get_raw_storage(lhs);

    if (std::holds_alternative<std::string>(l)) {
      return std::get<std::string>(l) + rhs.to_string();
    } else {
      return make_operation<Value>(std::plus{}, lhs, rhs);
    }
  }

  Value &operator+(std::string &value);

  friend Value operator-(const Value &lhs, const Value &rhs) {
    return make_operation<Value>(std::minus{}, lhs, rhs);
  }

  friend Value operator*(const Value &lhs, const Value &rhs) {
    return make_operation<Value>(std::multiplies{}, lhs, rhs);
  }

  friend Value operator/(const Value &lhs, const Value &rhs) {
    return make_operation<Value>(std::divides{}, lhs, rhs);
  }

  friend Value operator%(const Value &lhs, const Value &rhs) {
    return make_operation<Value>(std::modulus{}, lhs, rhs);
  }

  friend bool operator!=(const Value &lhs, const Value &rhs) {
    return make_operation<bool>(std::not_equal_to{}, lhs, rhs);
  }

  friend bool operator==(const Value &lhs, const Value &rhs) {
    return make_operation<bool>(std::equal_to{}, lhs, rhs);
  }

  friend bool operator<(const Value &lhs, const Value &rhs) {
    return make_operation<bool>(std::less{}, lhs, rhs);
  }

  friend bool operator<=(const Value &lhs, const Value &rhs) {
    return make_operation<bool>(std::less_equal{}, lhs, rhs);
  }

  friend bool operator>(const Value &lhs, const Value &rhs) {
    return make_operation<bool>(std::greater{}, lhs, rhs);
  }

  friend bool operator>=(const Value &lhs, const Value &rhs) {
    return make_operation<bool>(std::greater_equal{}, lhs, rhs);
  }

  Value &operator=(const Value &value);

  Value &operator++();
  Value &operator--();
  Value &operator++(int);
  Value &operator--(int);

  template <typename T>
  [[nodiscard]]
  T get_raw() {
    if (std::holds_alternative<T>(data)) {
      return std::get<T>(data);
    } else {
      throw std::runtime_error(std::format(
          "Error: cannot get raw value with type: {}", typeid(T).name()));
    }
  }

  [[nodiscard]]
  auto to_string() const -> const std::string;
};
} // namespace SLang

#endif // !VALUE_HPP
