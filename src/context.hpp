#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include "lexer.hpp"
#include "parser.hpp"

class Context {
private:
  Lexer lexer;
  Parser parser;

public:
  Context();
};

#endif // CONTEXT_HPP
