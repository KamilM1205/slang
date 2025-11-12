#include "ast.hpp"
#include "lexer.hpp"
#include <gtest/gtest.h>
#include <memory>

TEST(ASTreeTest, PrinterTest) {
  AST::ASTree tree;

  auto value =
      std::make_unique<AST::ValueExpr>(Token(0, 0, 0, TokenType::NUMBER, "23"));
  auto unary = std::make_unique<AST::UnaryExpr>(
      Token(0, 0, 0, TokenType::MINUS), std::move(value));
  auto binary = std::make_unique<AST::BinaryExpr>(
      std::make_unique<AST::ValueExpr>(Token(0, 0, 0, TokenType::NUMBER, "10")),
      Token(0, 0, 0, TokenType::PLUS), std::move(unary));

  tree.add_expression(std::move(binary));

  std::cout << tree.to_string() << std::endl;
}
