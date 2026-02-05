/**
 * @file error.hpp
 * @brief Хранит структуру токена и лексера
 * @details Хранит все типы данных реализующих токен, а также лексер
 * @author Kamil Meftahutdinov
 * @date 2026
 */

#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

#include "message.hpp"
#include "token.hpp"
#include <cstddef>
#include <string>
#include <vector>

// TODO: Rewrite to std::variant
// TODO: Check all the lexer for superflous copying objects

auto tok2str(const Token &token) -> std::string;
auto type2str(const TokenType token) -> std::string;

using TokenList = std::vector<Token>;

/* @class Lexer
 * @brief Класс реализующий функционал лексера для разбиения исходного текста на
 * массив токенов.
 */
class Lexer {
private:
  TokenList tokens;
  std::string text;
  size_t index; // index of cuurent position on text
  char curr_ch;
  size_t line;
  size_t column;

  MessageContainer *econ;

  friend class LexerTest; // Need for gtest to access private methods
                          // and fields

  char peek();
  void next();

  void next_line();
  [[nodiscard]]
  auto get_line() -> std::string;
  bool check_eol(char ch);
  void check_overflow();

  [[nodiscard("Return value must be used")]]
  bool escape_characters(std::string &literal);

  void read_identifier();

  void read_number();

  void read_string();
  void read_multi_string();

  void pass_comment();
  void pass_multi_comment();

  void addToken(TokenType type);
  void addToken(TokenType type, std::string value);

public:
  Lexer();
  Lexer(std::string &&text);
  Lexer(const std::string &text);

  void set_source(std::string &&text);
  void set_source(const std::string &text);

  void tokenize();
  [[nodiscard]]
  auto get_line(const Token &token) -> std::string;

  [[nodiscard]]
  auto getTokens() const -> const TokenList &;
};

#endif
