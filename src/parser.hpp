#ifndef PARSER_HPP
#define PARSER_HPP

#include "ast.hpp"
#include "error.hpp"
#include "lexer.hpp"
#include <initializer_list>

class Parser {
private:
  size_t index;
  Lexer lexer;
  ErrorContainer *econ;
  AST::ASTree tree;
  TokenList tlist;

  bool match(std::initializer_list<TokenType> token_types) const;
  bool peek(std::initializer_list<TokenType> token_types) const;
  auto curr_tok() const -> const Token &;
  void next();
  auto consume(TokenType type) -> Token;
  auto consume(TokenType type, std::string msg) -> Token;
  bool is_type();

  auto parse_value() -> AST::Expr_t;
  auto parse_fun_call() -> AST::Expr_t;
  auto parse_primary() -> AST::Expr_t;
  auto parse_unary() -> AST::Expr_t;
  auto parse_factor() -> AST::Expr_t;
  auto parse_term() -> AST::Expr_t;
  auto parse_increment_postfix() -> AST::Expr_t;
  auto parse_increment_prefix() -> AST::Expr_t;
  auto parse_compare() -> AST::Expr_t;
  auto parse_and() -> AST::Expr_t;
  auto parse_or() -> AST::Expr_t;
  auto parse_condition() -> AST::Expr_t;
  auto parse_ternary() -> AST::Expr_t;

  auto parse_expression() -> AST::Expr_t;

  auto parse_assign() -> AST::Expr_t;
  auto parse_define() -> AST::DefineExpr;
  auto parse_import() -> AST::ImportStmt;
  auto parse_expression_stmt() -> AST::Expr_t;
  auto parse_block() -> AST::BlockStmt;
  auto parse_arg_define() -> AST::ArgDefineExpr;
  auto parse_args() -> std::vector<AST::ArgDefineExpr>;
  auto parse_fn_define() -> AST::FnDefineStmt;
  auto parse_return() -> AST::ReturnStmt;
  auto parse_elif_stmt() -> AST::IfStmt;
  auto parse_if_stmt() -> AST::IfStmt;
  auto parse_while_stmt() -> AST::WhileStmt;
  auto parse_class_field(Token &identifier) -> AST::ClassField;
  auto parse_class_method(Token &identifier) -> AST::ClassFnDefineStmt;
  auto parse_class_block() -> AST::BlockStmt;
  auto parse_class_stmt() -> AST::ClassStmt;
  auto parse_interface_fn() -> AST::InterfaceFnDefineStmt;
  auto parse_interface_block() -> AST::BlockStmt;
  auto parse_interface_stmt() -> AST::InterfaceStmt;
  auto parse_for_stmt() -> AST::ForStmt;
  auto parse_stmt() -> AST::Expr_t;

public:
  Parser();
  Parser(std::string &&source);
  Parser(const std::string &source);

  void set_source(const std::string &source);
  void set_source(std::string &&source);

  void parse();
};

#endif // !PARSER_HPP
