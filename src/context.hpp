#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include "ast.hpp"
#include "interpreter.hpp"
#include "lexer.hpp"
#include "message.hpp"
#include "parser.hpp"

namespace SLang {
class SLContext {
private:
  AST::ASTree *tree;
  Lexer lexer;
  Parser parser;
  Interpreter interpreter;
  MessageContainer *econ;

public:
  SLContext();
  void parse(std::string &source);
  void execute();
  auto get_global_env() -> Environment &;
};
} // namespace SLang

#endif // CONTEXT_HPP
