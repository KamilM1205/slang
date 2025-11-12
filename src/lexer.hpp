#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

#include "error.hpp"
#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

// TODO: Rewrite to std::variant
// TODO: Rewrite to trailing return
// TODO: Check all the lexer for superflous copying objects

enum class NumberNotation {
  BIN,
  HEX,
  ORD,
};

enum class TokenType {
  NONE = 0,
  _EOF,
  // Keywords
  FOR,
  WHILE,
  IF,
  ELIF,
  ELSE,
  AND,
  OR,
  TRUE,
  FALSE,
  VAR,
  AUTO, // Not using directly. Used in parser when not specified type in var
        // stmt.
  INT,
  FLOAT,
  STRING,
  USTRING, // TODO: Add support for UTF-8 strings in future
  BOOL,
  FUNCTION,
  RETURN,
  IMPORT,
  CLASS,
  INTERFACE,
  IDENTIFIER,
  // Special characters
  NUMBER,
  LITTERAL, // ".."
  MLIT,     // [[ .. ]]
  COMMENT,  // //
  MCOMM,    // /* .. */

  QUEST,         // ?
  PERCENT,       // %
  COMMA,         // ,
  DOT,           // .
  PLUS,          // +
  INCREMENT,     // ++
  MINUS,         // -
  DECREMENT,     // --
  STAR,          // *
  SLASH,         // /
  BLK_BEGIN,     // {
  BLK_END,       // }
  BR_BEGIN,      // (
  BR_END,        // )
  BRACKET_BEGIN, // [
  BRACKET_END,   // ]
  SEMICOLON,     // ;
  COLON,         // :
  EQ,            // ==
  GR,            // >
  LS,            // <
  GE,            // >=
  LE,            // <=
  NOT,           // !
  NOT_EQ,        // !=
  SET            // =
};

extern const std::unordered_map<std::string, TokenType> keywords_table;

class Token {
private:
  TokenType type;
  std::string value;
  size_t _line, _column, _index;

public:
  Token() = default;
  Token(size_t line, size_t column, size_t index, TokenType type,
        std::string value)
      : _line(line), _column(column), _index(index), type(type), value(value) {}
  Token(size_t line, size_t column, size_t index, TokenType type)
      : Token(line, column, index, type, "") {}

  [[nodiscard]]
  auto line() const -> size_t {
    return _line;
  }
  [[nodiscard]]
  auto column() const -> size_t {
    return _column;
  }
  [[nodiscard]]
  auto index() const -> size_t {
    return _index;
  }

  bool operator==(const Token &a) const {
    return type == a.type && value == a.value;
  }

  bool operator!=(const Token &a) const {
    return type != a.type && value != a.value;
  }

  auto &operator+=(const char value) {
    this->value += value;
    return *this;
  }

  [[nodiscard]]
  auto getType() const -> TokenType {
    return type;
  };
  void setType(TokenType type) { this->type = type; }

  [[nodiscard]]
  auto getValue() const -> std::string {
    return value;
  }
  void setValue(std::string value) { this->value = value; }

  bool empty() { return type == TokenType::NONE && value.empty(); }
};

auto tok2str(const Token &token) -> std::string;
auto type2str(const TokenType token) -> std::string;

using TokenList = std::vector<Token>;

class Lexer {
private:
  TokenList tokens;
  std::string text;
  size_t index; // index of cuurent position on text
  char curr_ch;
  size_t line;
  size_t column;

  ErrorContainer *econ;

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
