#ifndef STDLIB_HPP
#define STDLIB_HPP

#include "context.hpp"
#include "interpreter.hpp"

namespace SLang {
class StdLib {
public:
  StdLib() = default;
  StdLib(StdLib &&) = default;
  StdLib(const StdLib &) = default;
  StdLib &operator=(StdLib &&) = default;
  StdLib &operator=(const StdLib &) = default;
  ~StdLib() = default;

  static void bind(SLContext &ctx);

private:
  static Value print(Environment &env, Args args);
  static Value println(Environment &env, Args args);
  static Value input(Environment &env, Args args);

  static Value to_int(Environment &env, Args args);
  static Value to_float(Environment &env, Args args);
  static Value to_string(Environment &env, Args args);
  static Value unref(Environment &env, Args args);
};
} // namespace SLang

#endif // !STDLIB_HPP
