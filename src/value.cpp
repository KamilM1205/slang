#include "value.hpp"
#include "token.hpp"
#include "utils.hpp"
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <variant>

auto SLang::Value::get_number_type(std::string value) -> ValueType {
  if (value.find('.') != std::string::npos) {
    return ValueType::FLOAT;
  }

  return ValueType::INT;
}

auto SLang::Value::string_to_int(std::string value) -> int32_t {
  int32_t base = 10;

  if (get_number_type(value) != ValueType::INT) {
    throw std::runtime_error("Expected int value.");
  }

  switch (get_base(value)) {
  case SLang::NumberNotation::BIN:
    base = 2;
    break;

  case NumberNotation::ORD:
    base = 10;
    break;

  case NumberNotation::HEX:
    base = 16;
    break;
  }

  return std::stoi(get_number_without_notation(value), nullptr, base);
}

auto SLang::Value::string_to_float(std::string value) -> float {
  if (get_number_type(value) != ValueType::FLOAT) {
    throw std::runtime_error("Expected float value.");
  }

  return std::stof(value);
}

auto SLang::Value::get_base(std::string value) -> NumberNotation {
  char notation;

  if (value.size() <= 2) {
    return NumberNotation::ORD;
  }

  notation = std::tolower(value[1]);

  if (value[0] == '0') {
    switch (notation) {
    case 'b':
      return NumberNotation::BIN;
    case 'x':
      return NumberNotation::HEX;
    default:
      return NumberNotation::ORD;
    }
  } else {
    return NumberNotation::ORD;
  }
}

// Return string without number notation
auto SLang::Value::get_number_without_notation(std::string value)
    -> std::string {
  if (type == ValueType::INT || type == ValueType::FLOAT) {
    if (get_base(value) != NumberNotation::ORD) {
      return value.substr(2);
    }
  }

  return value;
}

auto SLang::Value::storage_to_value_type(Storage &store) -> ValueType {
  ValueType ret;

  std::visit(type_match{
                 [&ret](std::monostate) { ret = ValueType::NONE; },
                 [&ret](AnyType) { ret = ValueType::Any; },
                 [&ret](int32_t) { ret = ValueType::INT; },
                 [&ret](float) { ret = ValueType::FLOAT; },
                 [&ret](bool) { ret = ValueType::BOOL; },
                 [&ret](std::string) { ret = ValueType::STRING; },
                 [&ret](std::shared_ptr<Value>) { ret = ValueType::REFERENCE; },
             },
             store);

  return ret;
}

auto SLang::Value::get_data() -> Storage & { return data; }

auto SLang::Value::type_to_string(ValueType type) -> std::string {
  switch (type) {
  case ValueType::NONE:
    return "void";
  case ValueType::Any:
    return "any";
  case ValueType::INT:
    return "int";
  case ValueType::FLOAT:
    return "float";
  case ValueType::BOOL:
    return "bool";
  case ValueType::STRING:
    return "string";
  case ValueType::REFERENCE:
    return "ref";
  }
}

auto SLang::Value::type_to_string() -> std::string {
  return type_to_string(type);
}

SLang::Value::Value(ValueType type) {
  this->type = type;

  switch (type) {
  case ValueType::NONE:
    data = std::monostate();
    break;
  case ValueType::Any:
    data = AnyType();
    break;
  case ValueType::INT:
    data = 0;
    break;
  case ValueType::FLOAT:
    data = 0.f;
    break;
  case ValueType::BOOL:
    data = false;
    break;
  case ValueType::STRING:
    data = "";
    break;
  case ValueType::REFERENCE:
    data = std::monostate();
    break;
  }
}

SLang::Value::Value(Token token) {
  switch (token.get_type()) {
  case TokenType::NUMBER:
    type = get_number_type(token.get_value());
    if (type == ValueType::INT) {
      data = string_to_int(token.get_value());
    } else {
      data = string_to_float(token.get_value());
    }
    break;
  case TokenType::LITTERAL:
    type = ValueType::STRING;
    data = token.get_value();
    break;
  case TokenType::TRUE:
    type = ValueType::BOOL;
    data = true;
    break;
  case TokenType::FALSE:
    type = ValueType::BOOL;
    data = false;
    break;
  case TokenType::IDENTIFIER:
    break;
  default:
    throw std::runtime_error("Unexpected type.");
  }
}

auto SLang::Value::get_type() -> ValueType { return type; }

auto SLang::Value::from_token_type(SLang::TokenType type) -> ValueType {
  switch (type) {
  case TokenType::STRING:
    return ValueType::STRING;
  case TokenType::FLOAT:
    return ValueType::FLOAT;
  case TokenType::INT:
    return ValueType::INT;
  case TokenType::BOOL:
    return ValueType::BOOL;
  case TokenType::IDENTIFIER:
    return ValueType::REFERENCE;
  default:
    throw std::runtime_error("Unexpected token type.");
  }
}

auto SLang::Value::get_raw_value() -> Value { return get_raw_storage(*this); }

SLang::Value &SLang::Value::operator+(std::string &value) {
  if (type != ValueType::STRING) {
    throw std::runtime_error("Expected string value for add.");
  }

  data = std::get<std::string>(data) + value;

  return *this;
}

SLang::Value &SLang::Value::operator++() {
  auto &data = (std::holds_alternative<RefType>(this->data))
                   ? get_last_ref(std::get<RefType>(this->data))->data
                   : this->data;
  std::visit(
      [](auto &d) {
        using T = std::decay_t<decltype(d)>;
        if constexpr (!can_increment<T>) {
          throw std::runtime_error(
              std::format("Cannot increment given type: {}", typeid(T).name()));
        } else {
          d++;
        }
      },
      data);

  return *this;
}

SLang::Value &SLang::Value::operator--() {
  auto &data = (std::holds_alternative<RefType>(this->data))
                   ? get_last_ref(std::get<RefType>(this->data))->data
                   : this->data;
  std::visit(
      [](auto &d) {
        using T = std::decay_t<decltype(d)>;
        if constexpr (!can_decrement<T>) {
          throw std::runtime_error(
              std::format("Cannot decrement given type: {}", typeid(T).name()));
        } else {
          d--;
        }
      },
      data);

  return *this;
}

SLang::Value &SLang::Value::operator++(int) {
  Value value = *this;
  ++(*this);
  return *this;
}

SLang::Value &SLang::Value::operator--(int) {
  Value value = *this;
  --(*this);
  return *this;
}

SLang::Value &SLang::Value::operator=(const Value &value) {
  if (std::holds_alternative<RefType>(data)) {
    auto l = get_last_ref(std::get<RefType>(data));
    Storage r = value.data;

    if (std::holds_alternative<RefType>(value.data)) {
      r = get_last_ref(std::get<RefType>(value.data));
    }

    std::visit(
        [](auto &lhs, auto &rhs) {
          if constexpr (requires { lhs = rhs; }) {
            lhs = rhs;
          } else {
            throw std::runtime_error(
                std::format("Cannot assign value with type: {}, to {}",
                            typeid(rhs).name(), typeid(lhs).name()));
          }
        },
        l->data, r);
  } else {
    type = value.type;
    std::visit(
        [](auto &lhs, auto &rhs) {
          if constexpr (requires { lhs = rhs; }) {
            lhs = rhs;
          } else {
            throw std::runtime_error(
                std::format("Cannot assign value with type: {}, to {}",
                            typeid(rhs).name(), typeid(lhs).name()));
          }
        },
        data, value.data);
  }

  return *this;
}

auto SLang::Value::to_string() const -> const std::string {
  std::string out;

  if (std::holds_alternative<int32_t>(data)) {
    out = std::to_string(std::get<int32_t>(data));
  } else if (std::holds_alternative<float>(data)) {
    out = std::to_string(std::get<float>(data));
  } else if (std::holds_alternative<bool>(data)) {
    out = std::get<bool>(data) ? "true" : "false";
  } else if (std::holds_alternative<std::string>(data)) {
    out = std::get<std::string>(data);
  } else if (std::holds_alternative<RefType>(data)) {
    out = std::get<std::shared_ptr<Value>>(data)->to_string();
  } else {
    out = "error";
  }

  return out;
}
