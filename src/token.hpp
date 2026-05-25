#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>
#include <unordered_map>

namespace SLang {

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

/*
 * @brif Хранит тип токена для каждого ключевого слова.
 */
extern const std::unordered_map<std::string, TokenType> keywords_table;

/*
 * @class Token
 * @brief Хранит тип токена, его значение а также информацию о его положении в
 * тексте
 */
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
  auto get_type() const -> TokenType {
    return type;
  };
  void set_type(TokenType type) { this->type = type; }

  [[nodiscard]]
  auto get_value() const -> std::string {
    return value;
  }
  void set_value(std::string value) { this->value = value; }

  bool empty() { return type == TokenType::NONE && value.empty(); }
};

} // namespace SLang

#endif // !TOKEN_HPP
