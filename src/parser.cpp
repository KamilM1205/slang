#include "parser.hpp"
#include "ast.hpp"
#include "errors.hpp"
#include "lexer.hpp"
#include "message.hpp"
#include "panic.hpp"
#include "token.hpp"
#include <format>
#include <initializer_list>
#include <memory>
#include <optional>
#include <vector>

// TODO: write comments for all parser's functions

SLang::Parser::Parser() {
  econ = MessageContainer::get_instance();
  index = 0;
}

void SLang::Parser::set_source(const std::string &source) {
  lexer->set_source(source);
}

void SLang::Parser::set_source(std::string &&source) {
  lexer->set_source(std::move(source));
}

bool SLang::Parser::match(std::initializer_list<TokenType> token_types) const {
  for (auto type : token_types) {
    if (type == curr_tok().get_type()) {
      return true;
    }
  }

  return false;
}

bool SLang::Parser::peek(std::initializer_list<TokenType> token_types) const {
  TokenType tt = lexer->getTokens()[index + 1].get_type();

  for (auto type : token_types) {
    if (tt == type) {
      return true;
    }
  }

  return false;
}

auto SLang::Parser::curr_tok() const -> const Token & {
  return lexer->getTokens()[index];
}

void SLang::Parser::next() {
  if (curr_tok().get_type() != TokenType::_EOF) {
    index++;
  } else {
    econ->add_msg(MessageType::ERR, ERROR_TOKENS_END);
  }
}

auto SLang::Parser::pervious() const -> const Token & {
  if (index > 0) {
    return lexer->getTokens()[index - 1];
  } else {
    econ->add_msg(MessageType::ERR, curr_tok(), ERROR_ALREADY_BEGIN);
    panic();
  }

  return curr_tok();
}

auto SLang::Parser::consume(TokenType type) -> Token {
  Token token = curr_tok();

  if (token.get_type() == type) {
    next();
    return token;
  }

  econ->add_msg(MessageType::ERR, curr_tok(), ERROR_EXPECTED_TOKEN,
                type2str(type), tok2str(token));

  panic();

  return Token();
}

auto SLang::Parser::consume(TokenType type, std::string msg) -> Token {
  Token token = curr_tok();

  if (token.get_type() == type) {
    next();
    return token;
  }

  econ->add_msg(MessageType::ERR, curr_tok(), msg);
  panic();

  return Token();
}

bool SLang::Parser::is_type() {
  if (match({TokenType::IDENTIFIER, TokenType::BOOL, TokenType::INT,
             TokenType::FLOAT, TokenType::STRING, TokenType::USTRING})) {
    return true;
  }

  return false;
}

auto SLang::Parser::parse_value() -> AST::Expr_t {
  AST::Expr_t expr;

  if (match({TokenType::TRUE, TokenType::FALSE, TokenType::NUMBER,
             TokenType::LITTERAL, TokenType::IDENTIFIER})) {
    expr = std::unique_ptr<AST::ValueExpr>(new AST::ValueExpr(curr_tok()));
    next();
  } else {
    econ->add_msg(MessageType::ERR, curr_tok(), ERROR_EXPECTED_VALUE,
                  tok2str(curr_tok()));
    panic();
  }

  return std::move(expr);
}

auto SLang::Parser::parse_fun_call() -> AST::Expr_t {
  Token ident = consume(TokenType::IDENTIFIER);
  std::vector<AST::Expr_t> args;

  consume(TokenType::BR_BEGIN);

  if (!match({TokenType::BR_END})) {
    args.push_back(std::move(parse_expression()));

    while (match({TokenType::COMMA})) {
      next();
      args.push_back(std::move(parse_expression()));
    }
  }

  consume(TokenType::BR_END);

  return std::unique_ptr<AST::FunCallExpr>(
      new AST::FunCallExpr(ident, std::move(args)));
}

auto SLang::Parser::parse_atom() -> AST::Expr_t {
  AST::Expr_t expr;

  if (match({TokenType::IDENTIFIER}) && peek({TokenType::BR_BEGIN})) {
    expr = parse_fun_call();
  } else if (match({TokenType::IDENTIFIER})) {
    expr = parse_value();
  } else {
    consume(TokenType::BR_BEGIN, ERROR_EXPRESSION);
    expr = std::unique_ptr<AST::GroupingExpr>(
        new AST::GroupingExpr(parse_expression()));
    consume(TokenType::BR_END, ERROR_EXPRESSION);
  }

  return std::move(expr);
}

auto SLang::Parser::parse_class_member() -> AST::Expr_t {
  if (peek({TokenType::DOT})) {
    auto left = std::unique_ptr<AST::ValueExpr>(new AST::ValueExpr(curr_tok()));
    next(); // Skip identifier
    next(); // Skip dot

    return std::unique_ptr<AST::ClassMemberGetter>(new AST::ClassMemberGetter(
        std::move(left), std::move(parse_class_member())));
  } else if (peek({TokenType::BR_BEGIN})) {
    auto fun_call = std::move(parse_fun_call());
    AST::Expr_t right;

    if (match({TokenType::DOT})) {
      next(); // Skip dot
      right = parse_class_member();

      return std::unique_ptr<AST::ClassMemberCall>(
          new AST::ClassMemberCall(std::move(fun_call), std::move(right)));
    } else {
      right = std::unique_ptr<AST::VoidStmt>(new AST::VoidStmt());
    }

    return std::unique_ptr<AST::ClassMemberCall>(
        new AST::ClassMemberCall(std::move(fun_call), std::move(right)));
  } else if (match({TokenType::IDENTIFIER})) {
    auto left = parse_value();
    auto right = std::unique_ptr<AST::VoidStmt>(new AST::VoidStmt());

    return std::unique_ptr<AST::ClassMemberGetter>(
        new AST::ClassMemberGetter(std::move(left), std::move(right)));
  } else {
    return std::unique_ptr<AST::VoidStmt>(new AST::VoidStmt());
  }
}

auto SLang::Parser::parse_class_member_access() -> AST::Expr_t {
  AST::Expr_t left = std::move(parse_atom());
  std::optional<AST::Expr_t> right = {};

  if (match({TokenType::DOT}) && peek({TokenType::IDENTIFIER})) {
    next();
    right = std::move(parse_class_member());
  }

  if (right.has_value()) {
    if (match({TokenType::SET})) {
      AST::Expr_t expr;

      next(); // Skip '='
      expr = parse_expression();

      return std::unique_ptr<AST::ClassMemberSetter>(new AST::ClassMemberSetter(
          std::unique_ptr<AST::ClassMemberAccess>(new AST::ClassMemberAccess(
              std::move(left), std::move(right.value()))),
          std::move(expr)));
    }

    return std::unique_ptr<AST::ClassMemberAccess>(
        new AST::ClassMemberAccess(std::move(left), std::move(right.value())));
  } else {
    return std::move(left);
  }
}

auto SLang::Parser::parse_primary() -> AST::Expr_t {
  AST::Expr_t expr;

  if (match({TokenType::IDENTIFIER, TokenType::NUMBER, TokenType::LITTERAL,
             TokenType::TRUE, TokenType::FALSE, TokenType::BR_BEGIN})) {
    if (match({TokenType::IDENTIFIER}) /* && peek({TokenType::BR_BEGIN})) */ ||
        (match({TokenType::BR_BEGIN}))) {
      expr = parse_class_member_access();
    } else {
      expr = parse_value();
    }
  } else {
    expr = parse_value();
  }

  return std::move(expr);
}

auto SLang::Parser::parse_unary() -> AST::Expr_t {
  Token op;
  AST::Expr_t expr;

  if (match({TokenType::PLUS, TokenType::MINUS, TokenType::NOT})) {
    op = curr_tok();
    next();
    expr = parse_unary();
  } else {
    return std::move(parse_primary());
  }

  return std::unique_ptr<AST::UnaryExpr>(
      new AST::UnaryExpr(op, std::move(expr)));
}

auto SLang::Parser::parse_increment_postfix() -> AST::Expr_t {
  Token op;
  AST::Expr_t expr = parse_unary();

  if (match({TokenType::INCREMENT, TokenType::DECREMENT})) {
    op = curr_tok();
    next();

    expr = std::unique_ptr<AST::UnaryExpr>(
        new AST::UnaryExpr(op, std::move(expr)));
  }

  return std::move(expr);
}

auto SLang::Parser::parse_increment_prefix() -> AST::Expr_t {
  Token op;
  AST::Expr_t expr;

  if (match({TokenType::INCREMENT, TokenType::DECREMENT})) {
    op = curr_tok();
    next();
    expr = parse_unary();

    expr = std::unique_ptr<AST::UnaryExpr>(
        new AST::UnaryExpr(op, std::move(expr)));
  } else {
    expr = parse_increment_postfix();
  }

  return std::move(expr);
}

auto SLang::Parser::parse_factor() -> AST::Expr_t {
  auto expr = parse_increment_prefix();
  Token op;
  AST::Expr_t right;

  while (match({TokenType::STAR, TokenType::SLASH, TokenType::PERCENT})) {
    op = curr_tok();
    next();
    right = parse_increment_prefix();

    expr = std::unique_ptr<AST::BinaryExpr>(
        new AST::BinaryExpr(std::move(expr), op, std::move(right)));
  }

  return std::move(expr);
}

auto SLang::Parser::parse_term() -> AST::Expr_t {
  auto expr = parse_factor();
  Token op;
  AST::Expr_t right;

  while (match({TokenType::PLUS, TokenType::MINUS})) {
    op = curr_tok();
    next();
    right = parse_factor();

    expr = std::unique_ptr<AST::BinaryExpr>(
        new AST::BinaryExpr(std::move(expr), op, std::move(right)));
  }

  return std::move(expr);
}

auto SLang::Parser::parse_compare() -> AST::Expr_t {
  auto expr = parse_term();
  Token op;
  AST::Expr_t right;

  while (match({TokenType::EQ, TokenType::NOT_EQ, TokenType::GR, TokenType::GE,
                TokenType::LS, TokenType::LE})) {
    op = curr_tok();
    next();
    right = parse_compare();

    expr = std::unique_ptr<AST::BinaryExpr>(
        new AST::BinaryExpr(std::move(expr), op, std::move(right)));
  }

  return std::move(expr);
}

auto SLang::Parser::parse_and() -> AST::Expr_t {
  auto expr = parse_compare();
  Token op;
  AST::Expr_t right;

  while (match({TokenType::AND})) {
    op = curr_tok();
    next();
    right = parse_and();

    expr = std::unique_ptr<AST::BinaryExpr>(
        new AST::BinaryExpr(std::move(expr), op, std::move(right)));
  }

  return std::move(expr);
}

auto SLang::Parser::parse_or() -> AST::Expr_t {
  auto expr = parse_and();
  Token op;
  AST::Expr_t right;

  while (match({TokenType::OR})) {
    op = curr_tok();
    next();
    right = parse_or();

    expr = std::unique_ptr<AST::BinaryExpr>(
        new AST::BinaryExpr(std::move(expr), op, std::move(right)));
  }

  return std::move(expr);
}

auto SLang::Parser::parse_condition() -> AST::Expr_t {
  return std::move(parse_or());
}

auto SLang::Parser::parse_ternary() -> AST::Expr_t {
  AST::Expr_t cond;

  if (match({TokenType::BR_BEGIN})) {
    cond = parse_condition();

    if (match({TokenType::QUEST})) {
      AST::Expr_t expr_true;
      AST::Expr_t expr_false;

      next(); // Skipping 'QUEST' token
      expr_true = parse_expression();
      consume(TokenType::COLON);
      expr_false = parse_expression();

      return std::unique_ptr<AST::TernaryExpr>(new AST::TernaryExpr(
          std::move(cond), std::move(expr_true), std::move(expr_false)));
    }
  } else {
    cond = parse_condition();
  }

  return std::move(cond);
}

auto SLang::Parser::parse_expression() -> AST::Expr_t {
  return std::move(parse_ternary());
}

auto SLang::Parser::parse_define() -> AST::DefineExpr {
  Token ident;
  Token value_type;
  std::optional<AST::Expr_t> expr;

  ident = consume(TokenType::IDENTIFIER);

  if (match({TokenType::COLON})) {
    next();
    if (is_type()) {
      value_type = curr_tok();
    } else {
      auto token = curr_tok();
      econ->add_msg(MessageType::ERR, curr_tok(), ERROR_EXPECTED_TOKEN,
                    "variable type", tok2str(curr_tok()));
      panic();
    }
    next();
  } else {
    value_type = Token(curr_tok().line(), curr_tok().column(),
                       curr_tok().index(), TokenType::AUTO);
  }

  if (match({TokenType::SET})) {
    next();
    expr = std::move(parse_expression());
  }

  if (!expr.has_value() && value_type.get_type() == TokenType::AUTO) {
    econ->add_msg(MessageType::ERR, curr_tok(), ERROR_VARIABLE_DEFINITION);
    panic();
  }

  consume(TokenType::SEMICOLON);

  if (expr.has_value()) {
    return AST::DefineExpr(ident, value_type, std::move(expr.value()));
  }

  return AST::DefineExpr(ident, value_type);
}

auto SLang::Parser::parse_assign() -> AST::Expr_t {
  Token ident;
  AST::Expr_t expr;

  if (match({TokenType::IDENTIFIER}) && peek({TokenType::SET})) {
    ident = curr_tok();
    next();
    next();
    expr = parse_expression();
    expr = std::unique_ptr<AST::AssignExpr>(
        new AST::AssignExpr(ident, std::move(expr)));
  } else {
    expr = parse_expression();
  }

  return std::move(expr);
}

auto SLang::Parser::parse_import() -> AST::ImportStmt {
  Token litteral;

  litteral = consume(TokenType::LITTERAL);

  consume(TokenType::SEMICOLON);

  return AST::ImportStmt(litteral);
}

auto SLang::Parser::parse_expression_stmt() -> AST::Expr_t {
  AST::Expr_t expr = parse_assign();

  consume(TokenType::SEMICOLON);

  return std::move(expr);
}

auto SLang::Parser::parse_block() -> AST::BlockStmt {
  std::vector<AST::Expr_t> stmts;
  AST::Expr_t stmt;

  Token block_begin = consume(TokenType::BLK_BEGIN);

  while (!match({TokenType::BLK_END})) {
    if (match({TokenType::_EOF})) {
      econ->add_msg(MessageType::ERR, ERROR_UNCLOSED_BLOCK, block_begin.line());
    }

    if (match({TokenType::RETURN})) {
      next();
      stmt =
          std::unique_ptr<AST::ReturnStmt>(new AST::ReturnStmt(parse_return()));
    } else {
      stmt = parse_stmt();
    }
    stmts.push_back(std::move(stmt));
  }

  consume(TokenType::BLK_END);

  return AST::BlockStmt(std::move(stmts));
}

auto SLang::Parser::parse_arg_define() -> AST::ArgDefineExpr {
  Token ident = consume(TokenType::IDENTIFIER);
  consume(TokenType::COLON);
  Token type;

  if (is_type()) {
    type = curr_tok();
    next();
  } else {
    econ->add_msg(MessageType::ERR, curr_tok(), ERROR_EXPECTED_TOKEN, "type",
                  tok2str(curr_tok()));
    panic();
  }

  return AST::ArgDefineExpr(ident, type);
}

auto SLang::Parser::parse_args() -> std::vector<AST::ArgDefineExpr> {
  std::vector<AST::ArgDefineExpr> args;

  do {
    if (!args.empty() && match({TokenType::COMMA})) {
      next();
    }

    args.push_back(parse_arg_define());
  } while (match({TokenType::COMMA}));

  return args;
}

auto SLang::Parser::parse_fn_define() -> AST::FnDefineStmt {
  Token ident = consume(TokenType::IDENTIFIER);
  std::vector<AST::ArgDefineExpr> args;
  std::optional<Token> ret_type;

  consume(TokenType::BR_BEGIN);

  if (!match({TokenType::BR_END})) {
    args = std::move(parse_args());
  }

  consume(TokenType::BR_END);

  if (match({TokenType::COLON})) {
    next();
    if (is_type()) {
      ret_type = curr_tok();
      next();
    } else {
      econ->add_msg(MessageType::ERR, curr_tok(), ERROR_EXPECTED_TOKEN, "type",
                    tok2str(curr_tok()));
    }
  }

  AST::BlockStmt block = std::move(parse_block());

  if (ret_type.has_value()) {
    return AST::FnDefineStmt(ident, std::move(args), std::move(block),
                             std::move(ret_type.value()));
  } else {
    return AST::FnDefineStmt(ident, std::move(args), std::move(block));
  }
}

auto SLang::Parser::parse_return() -> AST::ReturnStmt {
  AST::Expr_t expr = parse_expression();
  consume(TokenType::SEMICOLON);

  return AST::ReturnStmt(std::move(expr));
}

auto SLang::Parser::parse_elif_stmt() -> AST::IfStmt {
  AST::Expr_t cond;
  AST::BlockStmt block;

  consume(TokenType::BR_BEGIN);
  cond = parse_condition();
  consume(TokenType::BR_END);

  block = std::move(parse_block());

  return AST::IfStmt(std::move(cond), std::move(block), {}, {});
}

auto SLang::Parser::parse_if_stmt() -> AST::IfStmt {
  AST::Expr_t cond;
  AST::BlockStmt true_block;
  std::vector<std::unique_ptr<AST::IfStmt>> elif;
  std::optional<AST::BlockStmt> else_block;

  consume(TokenType::BR_BEGIN);
  cond = parse_condition();
  consume(TokenType::BR_END);

  true_block = std::move(parse_block());

  while (match({TokenType::ELIF})) {
    next();
    elif.push_back(
        std::unique_ptr<AST::IfStmt>(new AST::IfStmt(parse_elif_stmt())));
  }

  if (match({TokenType::ELSE})) {
    next();
    else_block = parse_block();
  }

  return AST::IfStmt(std::move(cond), std::move(true_block), std::move(elif),
                     std::move(else_block));
}

auto SLang::Parser::parse_while_stmt() -> AST::WhileStmt {
  AST::Expr_t cond;
  AST::BlockStmt block;

  consume(TokenType::BR_BEGIN);
  cond = parse_condition();
  consume(TokenType::BR_END);

  block = parse_block();

  return AST::WhileStmt(std::move(cond), std::move(block));
}

auto SLang::Parser::parse_for_stmt() -> AST::ForStmt {
  AST::Expr_t init;
  AST::Expr_t cond;
  AST::Expr_t step;
  AST::BlockStmt block;

  consume(TokenType::BR_BEGIN);

  if (match({TokenType::VAR})) {
    next();
    init =
        std::unique_ptr<AST::DefineExpr>(new AST::DefineExpr(parse_define()));
  } else {
    init = parse_assign();
    consume(TokenType::SEMICOLON);
  }

  cond = parse_condition();
  consume(TokenType::SEMICOLON);

  step = parse_expression();

  consume(TokenType::BR_END);

  block = parse_block();

  return AST::ForStmt(std::move(init), std::move(cond), std::move(step),
                      std::move(block));
}

auto SLang::Parser::parse_class_field(Token &identifier) -> AST::ClassField {
  Token type;
  bool is_private = false;

  consume(TokenType::COLON);

  if (!is_type()) {
    econ->add_msg(MessageType::ERR, curr_tok(), ERROR_EXPECTED_TOKEN,
                  "variable type", tok2str(curr_tok()));
  }

  type = curr_tok();
  next();

  consume(TokenType::SEMICOLON);

  if (identifier.get_value()[0] == '_') {
    is_private = true;
  }

  return AST::ClassField(identifier, is_private, type);
}

auto SLang::Parser::parse_class_method(Token &identifier)
    -> AST::ClassFnDefineStmt {
  bool is_private = false;
  std::vector<AST::ArgDefineExpr> args;
  std::optional<Token> ret_type;
  AST::BlockStmt block;

  consume(TokenType::BR_BEGIN);

  if (!match({TokenType::BR_END})) {
    args = std::move(parse_args());
  }

  consume(TokenType::BR_END);

  if (match({TokenType::COLON})) {
    next();
    if (is_type()) {
      ret_type = curr_tok();
      next();
    } else {
      econ->add_msg(MessageType::ERR, curr_tok(), ERROR_EXPECTED_TOKEN, "type",
                    tok2str(curr_tok()));
      panic();
    }
  }

  block = parse_block();

  if (identifier.get_value()[0] == '_') {
    is_private = true;
  }

  if (ret_type.has_value()) {
    return AST::ClassFnDefineStmt(identifier, is_private, args,
                                  ret_type.value(), std::move(block));
  } else {
    return AST::ClassFnDefineStmt(identifier, is_private, args,
                                  std::move(block));
  }
}

auto SLang::Parser::parse_class_block() -> AST::BlockStmt {
  std::vector<AST::Expr_t> stmts;
  consume(TokenType::BLK_BEGIN);

  while (!match({TokenType::BLK_END})) {
    Token ident;

    ident = consume(TokenType::IDENTIFIER);

    // Field
    if (match({TokenType::COLON})) {
      stmts.push_back(std::unique_ptr<AST::ClassField>(
          new AST::ClassField(parse_class_field(ident))));
    }
    // Method
    else if (match({TokenType::BR_BEGIN})) {
      stmts.push_back(std::unique_ptr<AST::ClassFnDefineStmt>(
          new AST::ClassFnDefineStmt(parse_class_method(ident))));
    } else {
      econ->add_msg(MessageType::ERR, curr_tok(), ERROR_UNEXPECTED_TOKEN,
                    tok2str(curr_tok()));
    }
  }

  consume(TokenType::BLK_END);

  return AST::BlockStmt(std::move(stmts));
}

auto SLang::Parser::parse_class_stmt() -> AST::ClassStmt {
  Token ident = consume(TokenType::IDENTIFIER);
  std::optional<Token> super;
  std::optional<std::vector<Token>> interfaces;
  AST::BlockStmt block;

  // Parse super class
  if (match({TokenType::COLON})) {
    next();

    super = consume(TokenType::IDENTIFIER);
  }

  if (match({TokenType::LS})) {
    interfaces = std::vector<Token>();
    next();
    do {
      if (curr_tok().get_type() == TokenType::IDENTIFIER) {
        interfaces->push_back(curr_tok());
        next();
      } else {
        econ->add_msg(MessageType::ERR, ERROR_EXPECTED_TOKEN, "interface name",
                      tok2str(curr_tok()));
        panic();
      }
    } while (match({TokenType::COMMA}));
  }

  block = std::move(parse_class_block());

  return AST::ClassStmt(ident, std::move(super), std::move(interfaces),
                        std::move(block));
}

auto SLang::Parser::parse_interface_fn() -> AST::InterfaceFnDefineStmt {
  Token ident = consume(TokenType::IDENTIFIER);
  std::vector<AST::ArgDefineExpr> args;
  std::optional<Token> ret_type;

  if (ident.get_value()[0] == '_') {
    Token perv = pervious();
    econ->add_msg(MessageType::ERR, perv.line(),
                  perv.column() - perv.get_value().size() + 1,
                  lexer->get_line(perv), ERROR_INTERFACE_METHOD_PRIVATE, "");
    panic();
  }

  consume(TokenType::BR_BEGIN);

  if (!match({TokenType::BR_END})) {
    args = std::move(parse_args());
  }

  consume(TokenType::BR_END);

  if (match({TokenType::COLON})) {
    next();
    if (is_type()) {
      ret_type = curr_tok();
      next();
    } else {
      econ->add_msg(MessageType::ERR, curr_tok(), ERROR_EXPECTED_TOKEN, "type",
                    tok2str(curr_tok()));
      panic();
    }
  }

  consume(TokenType::SEMICOLON);

  if (ret_type.has_value()) {
    return AST::InterfaceFnDefineStmt(ident, std::move(args),
                                      std::move(ret_type.value()));
  } else {
    return AST::InterfaceFnDefineStmt(ident, std::move(args));
  }
}

auto SLang::Parser::parse_interface_block() -> AST::BlockStmt {
  std::vector<AST::Expr_t> stmts;

  consume(TokenType::BLK_BEGIN);

  while (!match({TokenType::BLK_END})) {
    stmts.push_back(std::unique_ptr<AST::InterfaceFnDefineStmt>(
        new AST::InterfaceFnDefineStmt(parse_interface_fn())));
  }
  consume(TokenType::BLK_END);

  return AST::BlockStmt(std::move(stmts));
}

auto SLang::Parser::parse_interface_stmt() -> AST::InterfaceStmt {
  Token ident = consume(TokenType::IDENTIFIER);
  AST::BlockStmt block = parse_interface_block();

  return AST::InterfaceStmt(ident, std::move(block));
}

auto SLang::Parser::parse_stmt() -> AST::Expr_t {
  switch (curr_tok().get_type()) {
  case TokenType::VAR:
    next();
    return std::unique_ptr<AST::DefineExpr>(
        new AST::DefineExpr(parse_define()));

  case TokenType::IMPORT:
    next();
    return std::unique_ptr<AST::ImportStmt>(
        new AST::ImportStmt(parse_import()));

  case TokenType::FUNCTION:
    next();
    return std::unique_ptr<AST::FnDefineStmt>(
        new AST::FnDefineStmt(parse_fn_define()));

  case TokenType::BLK_BEGIN:
    return std::unique_ptr<AST::BlockStmt>(new AST::BlockStmt(parse_block()));

  case TokenType::IF:
    next();
    return std::unique_ptr<AST::IfStmt>(new AST::IfStmt(parse_if_stmt()));

  case TokenType::WHILE:
    next();
    return std::unique_ptr<AST::WhileStmt>(
        new AST::WhileStmt(parse_while_stmt()));

  case TokenType::FOR:
    next();
    return std::unique_ptr<AST::ForStmt>(new AST::ForStmt(parse_for_stmt()));

  case TokenType::INTERFACE:
    next();
    return std::unique_ptr<AST::InterfaceStmt>(
        new AST::InterfaceStmt(parse_interface_stmt()));

  case TokenType::CLASS:
    next();
    return std::unique_ptr<AST::ClassStmt>(
        new AST::ClassStmt(parse_class_stmt()));

  default:
    AST::Expr_t expr = parse_expression_stmt();
    if (expr != nullptr) {
      return std::unique_ptr<AST::ExprStmt>(new AST::ExprStmt(std::move(expr)));
    }

    std::string err =
        std::vformat(ERROR_UNEXPECTED_TOKEN,
                     std::make_format_args(
                         static_cast<const std::string>(tok2str(curr_tok()))));
    econ->add_msg(MessageType::ERR, curr_tok(), err);
    panic();
  }

  return std::unique_ptr<AST::VoidStmt>(new AST::VoidStmt());
}

SLang::AST::ASTree &SLang::Parser::parse(Lexer *lexer) {
  this->lexer = lexer;

  if (lexer->getTokens().empty()) {
    return tree;
  }

  while (curr_tok().get_type() != TokenType::_EOF) {
    tree.add_expression(parse_stmt());
  }

  // std::cout << tree.to_string() << std::endl;

  return tree;
}
