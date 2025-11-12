#ifndef UTILS_HPP
#define UTILS_HPP

#define cross_inline __attribute__((always_inline)) inline

#define getter(type, name)                                                     \
protected:                                                                     \
  type _##name;                                                                \
                                                                               \
public:                                                                        \
  type &name() { return _##name; }

#endif // !UTILS_HPP
