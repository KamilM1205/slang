#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include "lexer.hpp"
#include "message.hpp"
#include "parser.hpp"

class SLContext {
private:
  Lexer lexer;
  Parser parser;
  MessageContainer *econ;

public:
  SLContext();
  void parse(std::string &source);
};

#endif // CONTEXT_HPP
