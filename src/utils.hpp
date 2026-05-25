#ifndef UTILS_HPP
#define UTILS_HPP

#include <cassert>
#include <iostream> // IWYU pragma: keep
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
} // namespace SLang

#endif // !UTILS_HPP
