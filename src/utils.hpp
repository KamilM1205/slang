#ifndef UTILS_HPP
#define UTILS_HPP

#include <cassert>
#include <iostream> // IWYU pragma: keep
#include <stdexcept>
#include <string_view>
#include <type_traits>

#define cross_inline __attribute__((always_inline)) inline

/*
 * @brief Макрос для защиты полей AST от перезаписи
 * Макрос создает защищенное поле и его getter для возможности чтения полей из
 * элемента AST дерева, но без возможности записи, так как AST деревья -
 * иммутабельны.
 */
#define getter(type, name)                                                     \
protected:                                                                     \
  type _##name;                                                                \
                                                                               \
public:                                                                        \
  type &name() { return _##name; }

namespace SLang {
/*
 * @brief Match-like для std::visit
 */
template <class... Ts> struct type_match : Ts... {
  using Ts::operator()...;
};

template <typename> struct always_false : std::false_type {};

template <typename T> constexpr bool always_false_v = always_false<T>::value;

template <typename T>
concept can_increment = requires(T val) { val++; };

template <typename T>
concept can_decrement = requires(T val) { val--; };

template <typename T> struct TypeError;

#define SLAssert(cond, msg)                                                    \
  do {                                                                         \
    if (!(cond)) {                                                             \
      std::cerr << msg << std::endl;                                           \
      std::abort();                                                            \
    }                                                                          \
  } while (0)

constexpr int cto_int(std::string_view str) {
  if (str.empty()) {
    throw std::invalid_argument("Empty string");
  }

  int result = 0;
  size_t i = 0;
  bool negative = false;

  // Handle sign
  if (str[0] == '-') {
    negative = true;
    i++;
  } else if (str[0] == '+') {
    i++;
  }

  if (i == str.size()) {
    throw std::invalid_argument("String contains only a sign");
  }

  // Process digits
  for (; i < str.size(); ++i) {
    char c = str[i];
    if (c < '0' || c > '9') {
      throw std::invalid_argument("Non-numeric character encountered");
    }

    // Simple base-10 accumulation
    result = result * 10 + (c - '0');
  }

  return negative ? -result : result;
}

} // namespace SLang

#endif // !UTILS_HPP
