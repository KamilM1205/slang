#ifndef VALUE_HPP
#define VALUE_HPP

#include "lexer.hpp"
#include <atomic>
#include <cctype>
#include <cstddef>
#include <string>
#include <type_traits>

namespace SLang {
template <typename T> class Value {
protected:
  std::atomic<size_t> _id;

private:
  T data;

  auto get_base(std::string value) -> NumberNotation {
    char notation;

    if (value.size() <= 2) {
      return NumberNotation::ORD;
    }

    notation = std::tolower(value[1]);

    if (value[0] == '0') {
      switch (notation) {
      case 'b':
        return NumberNotation::BIN;
      case 'f':
        return NumberNotation::HEX;
      default:
        return NumberNotation::ORD;
      }
    }
  }

  // Return string without number notation
  auto get_no_notation(std::string value) -> std::string {}

  void str2type(std::string value) {
    if constexpr (std::is_same_v<T, int>) {
      data = std::stoi(get_no_notation(value), nullptr, get_base(value));
    } else if constexpr (std::is_same_v<T, float>) {

    } else if constexpr (std::is_same_v<T, std::string>) {

    } else {
    }
  }

public:
  Value() : _id(_id++) {}
  Value(T value) : _id(_id++) { data = value; }
  Value(std::string value) : _id(_id++) {}
  Value(Value &value) : _id(_id++), data(value.data) {}

  auto id() -> size_t { return _id; }
};
} // namespace SLang

#endif // !VALUE_HPP
