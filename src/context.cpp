#include "context.hpp"
#include "message.hpp"
#include "parser.hpp"

SLContext::SLContext() {
  econ = MessageContainer::get_instance();
  lexer = Lexer();
  parser = Parser();
}

void SLContext::parse(std::string &source) {
  econ->set_src(&source);
  lexer.set_source(source);
  lexer.tokenize();
  parser.parse(&lexer);
}
