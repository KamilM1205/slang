#include "stdlib.hpp"
#include "context.hpp"
#include "interpreter.hpp"
#include "value.hpp"
#include <iostream>
#include <sstream>
#include <string>

void SLang::StdLib::bind(SLang::SLContext &ctx) {
  ctx.get_global_env().add_fn(
      Function("print", {{"out", ValueType::Any}}, StdLib::print));
  ctx.get_global_env().add_fn(
      Function("println", {{"out", ValueType::Any}}, StdLib::println));
  ctx.get_global_env().add_fn(Function("input", {}, StdLib::input));

  ctx.get_global_env().add_fn(
      Function("toint", {{"x", ValueType::Any}}, StdLib::to_int));
  ctx.get_global_env().add_fn(
      Function("tofloat", {{"x", ValueType::Any}}, StdLib::to_float));
  ctx.get_global_env().add_fn(
      Function("tostring", {{"x", ValueType::Any}}, StdLib::to_string));
  ctx.get_global_env().add_fn(
      Function("unref", {{"ref", ValueType::REFERENCE}}, StdLib::unref));
}

SLang::Value SLang::StdLib::print(Environment &env, Args args) {
  std::stringstream ss;

  for (auto &arg : args) {
    ss << arg.to_string();
  }

  std::cout << ss.str();

  return Value();
}

SLang::Value SLang::StdLib::println(Environment &env, Args args) {
  print(env, args);

  std::cout << std::endl;

  return Value();
}

SLang::Value SLang::StdLib::input(Environment &env, Args args) {
  std::string i;

  std::cin >> i;

  return Value(i);
}

SLang::Value SLang::StdLib::to_int(SLang::Environment &env, Args args) {
  return std::stoi(args[0].to_string());
}

SLang::Value SLang::StdLib::to_float(SLang::Environment &env, Args args) {
  return std::stof(args[0].to_string());
}

SLang::Value SLang::StdLib::to_string(SLang::Environment &env, Args args) {
  return Value(args[0].to_string());
}

SLang::Value SLang::StdLib::unref(SLang::Environment &env, Args args) {

  return args[0].get_data();
}
