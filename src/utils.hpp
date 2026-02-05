#ifndef UTILS_HPP
#define UTILS_HPP

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

#endif // !UTILS_HPP
