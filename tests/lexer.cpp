#include "lexer.hpp"
#include <cstdio>
#include <gtest/gtest.h>
#include <string>
#include <unordered_map>
#include <vector>

class LexerTest : public ::testing::Test {
private:
  Lexer lexer;

protected:
  void setText(std::string &&text) { lexer.set_source(std::move(text)); }

  TokenList getTokens() { return lexer.getTokens(); }

  void tokenize() { lexer.tokenize(); }

public:
  void reset() { lexer = Lexer(); }
};

std::string tokenTypeToString(TokenType type) {
  static const std::unordered_map<TokenType, std::string> tokenTypeStrings = {
      {TokenType::NONE, "NONE"},
      // Keywords
      {TokenType::FOR, "FOR"},
      {TokenType::WHILE, "WHILE"},
      {TokenType::IF, "IF"},
      {TokenType::ELIF, "ELIF"},
      {TokenType::ELSE, "ELSE"},
      {TokenType::AND, "AND"},
      {TokenType::OR, "OR"},
      {TokenType::TRUE, "TRUE"},
      {TokenType::FALSE, "FALSE"},
      {TokenType::INT, "INT"},
      {TokenType::FLOAT, "FLOAT"},
      {TokenType::STRING, "STRING"},
      {TokenType::USTRING, "USTRING"},
      {TokenType::BOOL, "BOOL"},
      {TokenType::FUNCTION, "FUNCTION"},
      {TokenType::RETURN, "RETURN"},
      {TokenType::IMPORT, "IMPORT"},
      {TokenType::CLASS, "CLASS"},
      {TokenType::INTERFACE, "INTERFACE"},
      {TokenType::VAR, "VAR"},
      {TokenType::IDENTIFIER, "IDENTIFIER"},
      {TokenType::NUMBER, "NUMBER"},
      {TokenType::LITTERAL, "LITTERAL"},
      {TokenType::MLIT, "MLIT"},
      {TokenType::COMMENT, "COMMENT"},
      {TokenType::MCOMM, "MCOMM"},
      {TokenType::COMMA, "COMMA"},
      {TokenType::DOT, "DOT"},
      {TokenType::PLUS, "PLUS"},
      {TokenType::MINUS, "MINUS"},
      {TokenType::STAR, "STAR"},
      {TokenType::SLASH, "SLASH"},
      {TokenType::BLK_BEGIN, "BLK_BEGIN"},
      {TokenType::BLK_END, "BLK_END"},
      {TokenType::BR_BEGIN, "BR_BEGIN"},
      {TokenType::BR_END, "BR_END"},
      {TokenType::SEMICOLON, "SEMICOLON"},
      {TokenType::COLON, "COLON"},
      {TokenType::EQ, "EQ"},
      {TokenType::GR, "GR"},
      {TokenType::LS, "LS"},
      {TokenType::GE, "GE"},
      {TokenType::LE, "LE"},
      {TokenType::NOT, "NOT"},
      {TokenType::SET, "SET"},
      {TokenType::_EOF, "EOF"},
  };

  auto it = tokenTypeStrings.find(type);
  if (it != tokenTypeStrings.end()) {
    return it->second;
  }
  return "UNKNOWN";
}

void printTokens(TokenList list) {
  std::cout << "Tokens: ";
  for (auto token : list) {
    std::cout << "type: " << tokenTypeToString(token.getType())
              << ", value: " << token.getValue() << "; ";
  }

  std::cout << std::endl;
}

TEST_F(LexerTest, Math) {
  std::string text = "var a = (5 + 1.5) / 2;";
  reset();
  setText(std::move(text));
  tokenize();

  std::vector<Token> tokens{
      Token(1, 3, 2, TokenType::VAR),
      Token(1, 5, 4, TokenType::IDENTIFIER, "a"),
      Token(1, 7, 6, TokenType::SET),
      Token(1, 8, 7, TokenType::BR_BEGIN),
      Token(1, 9, 8, TokenType::NUMBER, "5"),
      Token(1, 11, 9, TokenType::PLUS),
      Token(1, 15, 14, TokenType::NUMBER, "1.5"),
      Token(1, 16, 15, TokenType::BR_END),
      Token(1, 18, 17, TokenType::SLASH),
      Token(1, 20, 19, TokenType::NUMBER, "2"),
      Token(1, 21, 20, TokenType::SEMICOLON),
      Token(1, 21, 20, TokenType::_EOF),
  };

  printTokens(getTokens());

  ASSERT_EQ(getTokens().size(), tokens.size());
  ASSERT_EQ(static_cast<std::vector<Token>>(getTokens()), tokens);
}

// TEST_F(LexerTest, Notation) {
//   std::string text = "0x4F 0b1010 0X3545 0B1001";
//   reset();
//   setText(std::move(text));
//   tokenize();
//
//   std::vector<Token> tokens{
//       Token(1, 4, 3, TokenType::NUMBER, "0x4F"),
//       Token(1, 12, 11, TokenType::NUMBER, "0b1010"), Token(TokenType::NUMBER,
//       "0X3545"), Token(TokenType::NUMBER, "0B1001"), Token(TokenType::_EOF)};
//
//   printTokens(getTokens());
//
//   ASSERT_EQ(getTokens().size(), tokens.size());
//   ASSERT_EQ(getTokens(), tokens);
// }
//
// TEST_F(LexerTest, escape_sequense) {
//   std::string text = "\"Hello,\\n\\tworld!\\b\"";
//   reset();
//   setText(text);
//   tokenize();
//   std::string test = "Hello,\n\tworld!\b";
//
//   printTokens(getTokens());
//
//   ASSERT_EQ(getTokens()[0].getValue(), test);
// }
//
// TEST_F(LexerTest, Tokenized) {
//   std::string text = R"(
// /*
// It's multiline comment. It don't be in token list.
// */
// import "awesome";
//
// interface Inter1 {
//   method1(arg1: int): string;
//   method2();
// }
//
// class SomeStuff: Base < Inter1, Inter2 {
//   field: int; // public field
//   _field1: int; // private field
//   // Contructor
//   init() {
//     field = 5;
//     _field1 = 6;
//   }
//
//   print_data() {
//     print("_field1 = {}", _field1);
//   }
// }
//
// fn entry() {
//   var myclass: SomeStuff = SomeStuff();
//   SomeStuff.print();
// }
//   )";
//
//   // It's hell :c
//   std::vector<Token> output{Token(TokenType::IMPORT),
//                             Token(TokenType::LITTERAL, "awesome"),
//                             Token(TokenType::SEMICOLON),
//                             Token(TokenType::INTERFACE),
//                             Token(TokenType::IDENTIFIER, "Inter1"),
//                             Token(TokenType::BLK_BEGIN),
//                             Token(TokenType::IDENTIFIER, "method1"),
//                             Token(TokenType::BR_BEGIN),
//                             Token(TokenType::IDENTIFIER, "arg1"),
//                             Token(TokenType::COLON),
//                             Token(TokenType::INT),
//                             Token(TokenType::BR_END),
//                             Token(TokenType::COLON),
//                             Token(TokenType::STRING),
//                             Token(TokenType::SEMICOLON),
//                             Token(TokenType::IDENTIFIER, "method2"),
//                             Token(TokenType::BR_BEGIN),
//                             Token(TokenType::BR_END),
//                             Token(TokenType::SEMICOLON),
//                             Token(TokenType::BLK_END),
//                             Token(TokenType::CLASS),
//                             Token(TokenType::IDENTIFIER, "SomeStuff"),
//                             Token(TokenType::COLON),
//                             Token(TokenType::IDENTIFIER, "Base"),
//                             Token(TokenType::LS),
//                             Token(TokenType::IDENTIFIER, "Inter1"),
//                             Token(TokenType::COMMA),
//                             Token(TokenType::IDENTIFIER, "Inter2"),
//                             Token(TokenType::BLK_BEGIN),
//                             Token(TokenType::IDENTIFIER, "field"),
//                             Token(TokenType::COLON),
//                             Token(TokenType::INT),
//                             Token(TokenType::SEMICOLON),
//                             Token(TokenType::IDENTIFIER, "_field1"),
//                             Token(TokenType::COLON),
//                             Token(TokenType::INT),
//                             Token(TokenType::SEMICOLON),
//                             Token(TokenType::IDENTIFIER, "init"),
//                             Token(TokenType::BR_BEGIN),
//                             Token(TokenType::BR_END),
//                             Token(TokenType::BLK_BEGIN),
//                             Token(TokenType::IDENTIFIER, "field"),
//                             Token(TokenType::SET),
//                             Token(TokenType::NUMBER, "5"),
//                             Token(TokenType::SEMICOLON),
//                             Token(TokenType::IDENTIFIER, "_field1"),
//                             Token(TokenType::SET),
//                             Token(TokenType::NUMBER, "6"),
//                             Token(TokenType::SEMICOLON),
//                             Token(TokenType::BLK_END),
//                             Token(TokenType::IDENTIFIER, "print_data"),
//                             Token(TokenType::BR_BEGIN),
//                             Token(TokenType::BR_END),
//                             Token(TokenType::BLK_BEGIN),
//                             Token(TokenType::IDENTIFIER, "print"),
//                             Token(TokenType::BR_BEGIN),
//                             Token(TokenType::LITTERAL, "_field1 = {}"),
//                             Token(TokenType::COMMA),
//                             Token(TokenType::IDENTIFIER, "_field1"),
//                             Token(TokenType::BR_END),
//                             Token(TokenType::SEMICOLON),
//                             Token(TokenType::BLK_END),
//                             Token(TokenType::BLK_END),
//                             Token(TokenType::FUNCTION),
//                             Token(TokenType::IDENTIFIER, "entry"),
//                             Token(TokenType::BR_BEGIN),
//                             Token(TokenType::BR_END),
//                             Token(TokenType::BLK_BEGIN),
//                             Token(TokenType::VAR),
//                             Token(TokenType::IDENTIFIER, "myclass"),
//                             Token(TokenType::COLON),
//                             Token(TokenType::IDENTIFIER, "SomeStuff"),
//                             Token(TokenType::SET),
//                             Token(TokenType::IDENTIFIER, "SomeStuff"),
//                             Token(TokenType::BR_BEGIN),
//                             Token(TokenType::BR_END),
//                             Token(TokenType::SEMICOLON),
//                             Token(TokenType::IDENTIFIER, "SomeStuff"),
//                             Token(TokenType::DOT),
//                             Token(TokenType::IDENTIFIER, "print"),
//                             Token(TokenType::BR_BEGIN),
//                             Token(TokenType::BR_END),
//                             Token(TokenType::SEMICOLON),
//                             Token(TokenType::BLK_END),
//                             Token(TokenType::_EOF)};
//   reset();
//   setText(text);
//
//   tokenize();
//
//   printTokens(getTokens());
//
//   ASSERT_EQ(getTokens().size(), output.size());
//
//   ASSERT_EQ(static_cast<std::vector<Token>>(getTokens()), output);
// }
