#include "context.hpp"
#include "interpreter.hpp"
#include "message.hpp"
#include "parser.hpp"
#include <cstddef>

SLang::SLContext::SLContext() {
  econ = MessageContainer::get_instance();
  lexer = Lexer();
  parser = Parser();
}

void SLang::SLContext::parse(std::string &source) {
  econ->set_src(&source);
  lexer.set_source(source);
  lexer.tokenize();
  tree = &parser.parse(&lexer);
  interpreter.set_tree(tree);
}

void SLang::SLContext::execute() { interpreter.execute(); }

auto SLang::SLContext::get_global_env() -> Environment & {
  return interpreter.get_global_env();
}
