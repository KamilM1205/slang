#include "lexer.hpp"
#include "errors.hpp"
#include "message.hpp"
#include "panic.hpp"
#include "utils.hpp"
#include <cctype>
#include <cstdint>
#include <string>
#include <unordered_map>

const char END_OF_STREAM = '\0';

auto SLang::type2str(TokenType type) -> std::string {
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
      {TokenType::AUTO, "AUTO"},
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
      {TokenType::INCREMENT, "INCREMENT"},
      {TokenType::DECREMENT, "DECREMENT"},
      {TokenType::PLUS, "PLUS"},
      {TokenType::MINUS, "MINUS"},
      {TokenType::STAR, "STAR"},
      {TokenType::PERCENT, "PERCENT"},
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

auto SLang::tok2str(const Token &token) -> std::string {
  return type2str(token.get_type());
}

const std::unordered_map<std::string, SLang::TokenType> SLang::keywords_table =
    {
        {"for", TokenType::FOR},       {"while", TokenType::WHILE},
        {"if", TokenType::IF},         {"elif", TokenType::ELIF},
        {"else", TokenType::ELSE},     {"and", TokenType::AND},
        {"or", TokenType::OR},         {"true", TokenType::TRUE},
        {"false", TokenType::FALSE},   {"var", TokenType::VAR},
        {"int", TokenType::INT},       {"float", TokenType::FLOAT},
        {"string", TokenType::STRING}, {"ustring", TokenType::USTRING},
        {"bool", TokenType::BOOL},     {"fn", TokenType::FUNCTION},
        {"return", TokenType::RETURN}, {"import", TokenType::IMPORT},
        {"class", TokenType::CLASS},   {"interface", TokenType::INTERFACE},
};

SLang::Lexer::Lexer() {
  econ = MessageContainer::get_instance();
  index = 0;
  column = 1;
  line = 1;
}

SLang::Lexer::Lexer(std::string &&text) : Lexer() {
  this->text = std::move(text);
}

SLang::Lexer::Lexer(const std::string &text) : Lexer() { this->text = text; }

void SLang::Lexer::set_source(std::string &&text) {
  this->text = std::move(text);
}

void SLang::Lexer::set_source(const std::string &text) { this->text = text; }

auto SLang::Lexer::getTokens() const -> const TokenList & { return tokens; }

cross_inline void SLang::Lexer::addToken(TokenType type) {
  tokens.push_back(Token(line, column, index, type));
}

cross_inline void SLang::Lexer::addToken(TokenType type, std::string value) {
  tokens.push_back(Token(line, column, index, type, value));
}

cross_inline void SLang::Lexer::check_overflow() {
  if (index + 1 >= SIZE_MAX) {
    econ->add_msg(MessageType::ERR, ERROR_INDEX_OVERFLOW);
    panic();
  }
}

cross_inline char SLang::Lexer::peek() { return text[index + 1]; }

void SLang::Lexer::next() {
  check_overflow();

  index++;

  column++;

  if (text[index] == '\n') {
    next_line();
  }

  curr_ch = text[index];
}

void SLang::Lexer::next_line() {
  column = 0;
  if (line + 1 == SIZE_MAX) {
    econ->add_msg(MessageType::ERR, ERROR_LINE_OVERFLOW);
    panic();
  }

  line++;
}

std::string SLang::Lexer::get_line() {
  size_t count = 0;

  for (auto i = index - column + 1; i < text.size(); i++) {
    if (text[i] == '\n') {
      break;
    }
    count++;
  }

  return text.substr((index - column + 1), count);
}

cross_inline static bool is_digit(char ch) { return ch >= '0' && ch <= '9'; }

cross_inline static bool is_alpha(char ch) {
  return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}

cross_inline static bool is_alphadigit(char ch) {
  return isdigit(ch) || isalpha(ch);
}

cross_inline bool SLang::Lexer::check_eol(char ch) {
  if (ch == END_OF_STREAM) {
    econ->add_msg(MessageType::ERR, line, column, ERROR_UNEXPECTED_EOF);
    return true;
  } else if (peek() == '\n') {
    econ->add_msg(MessageType::ERR, line, column, get_line(),
                  ERROR_UNEXPECTED_EOL);
    return true;
  }

  return false;
}

void SLang::Lexer::read_identifier() {
  std::string identifier;
  identifier += curr_ch;

  while (true) {
    if (is_alphadigit(peek()) || peek() == '_') {
      next();
      identifier += curr_ch;
    } else {
      break;
    }
  }

  auto it = keywords_table.find(identifier);
  if (it != keywords_table.end()) {
    addToken(it->second);
  } else {
    addToken(TokenType::IDENTIFIER, identifier);
  }
}

cross_inline static bool check_hex(char ch) {
  return ((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f') ||
          (ch >= 'A' && ch <= 'F'));
}

cross_inline static bool check_bin(char ch) { return (ch == '0' || ch == '1'); }

cross_inline void SLang::Lexer::read_number() {
  std::string number;
  NumberNotation notation = NumberNotation::ORD;

  number += curr_ch;

  if (curr_ch == '0') {
    switch (peek()) {
    case 'b':
    case 'B':
      notation = NumberNotation::BIN;
      break;
    case 'x':
    case 'X':
      notation = NumberNotation::HEX;
      break;

    default:
      goto parse_number;
    }

    next();
    number += curr_ch;
  }

parse_number:
  while (true) {
    if (check_eol(curr_ch)) {
      return;
    }

    if (notation == NumberNotation::BIN) {
      if (check_bin(peek())) {
        next();
        number += curr_ch;
      } else if (!is_alphadigit(peek())) {
        break;
      } else {
        econ->add_msg(MessageType::ERR, line, column + 1, get_line(),
                      ERROR_EXPECTED_BIN);
        return;
      }
    } else if (notation == NumberNotation::HEX) {
      if (check_hex(peek())) {
        next();
        number += curr_ch;
      } else if (!is_alphadigit(peek())) {
        break;
      } else {
        econ->add_msg(MessageType::ERR, line, column + 1, get_line(),
                      ERROR_EXPECTED_HEX);
        return;
      }
    } else {
      if (is_digit(peek()) || peek() == '.') {
        next();
        number += curr_ch;
      } else if (is_alpha(peek())) {
        econ->add_msg(MessageType::ERR, line, column + 1, get_line(),
                      ERROR_NUMBER_EXPECTED);
        return;
      } else {
        break;
      }
    }
  }

  addToken(TokenType::NUMBER, number);
}

bool SLang::Lexer::escape_characters(std::string &literal) {
  if (curr_ch == '\\') {
    char esc = peek();

    if (check_eol(esc)) {
      return false;
    }

    switch (esc) {
    case 'n': // new line
      literal += '\n';
      break;
    case 'r': // carriage return
      literal += '\r';
      break;
    case 't': // tab
      literal += '\t';
      break;
    case 'v': // vertical tab
      literal += '\v';
      break;
    case 'b': // backspace
      literal += '\b';
      break;
    case '"': // escape "
      literal += '\"';
      break;
    case '\\':
      literal += '\\';
      break;

    default:
      econ->add_msg(MessageType::ERR, line, column, get_line(),
                    ERROR_UNEXPECTED_ESC);
      goto exit;
      break;
    }

    if (!check_eol(peek())) {
      next();
    } else {
      return false;
    }

    return true;
  }

exit:
  return false;
}

cross_inline void SLang::Lexer::read_string() {
  std::string literal;

  while (true) {
    if (check_eol(peek())) {
      return;
    }

    next();
    if (curr_ch != '"') {
      if (!escape_characters(literal)) {
        literal += curr_ch;
      }
    } else {
      addToken(TokenType::LITTERAL, literal);
      break;
    }
  }
}

cross_inline void SLang::Lexer::read_multi_string() {
  std::string literal;
  std::string start_line = get_line();
  size_t ccolumn = column, cline = line;

  while (true) {
    if (peek() == END_OF_STREAM) {
      econ->add_msg(MessageType::ERR, cline, ccolumn, start_line,
                    ERROR_UNCLOSED_MULTISTRING);
      econ->add_msg(MessageType::ERR, line, column, ERROR_UNEXPECTED_EOF);
      panic();
      break;
    }

    next();
    if (curr_ch == ']') {
      if (peek() == ']') {
        addToken(TokenType::LITTERAL, literal);
        next();
        break;
      }
    }

    if (!escape_characters(literal)) {
      literal += curr_ch;
    }
  }
}

cross_inline void SLang::Lexer::pass_comment() {
  while (true) {
    if (peek() == '\n') {
      next();
      break;
    } else if (peek() == END_OF_STREAM) {
      break;
    }
    next();
  }
}

cross_inline void SLang::Lexer::pass_multi_comment() {
  std::string start_line = get_line();
  size_t cline = line, ccolumn = column;
  next();

  while (true) {
    if (peek() == END_OF_STREAM) {
      econ->add_msg(MessageType::ERR, cline, ccolumn, start_line,
                    ERROR_UNCLOSED_MULTICOMMENT);
      econ->add_msg(MessageType::ERR, line, column, ERROR_UNEXPECTED_EOF);
      panic();
    }
    next();
    if (curr_ch == '*' && peek() == '/') {
      next();
      break;
    }
  }
}

auto SLang::Lexer::get_line(const Token &token) -> std::string {
  size_t count = 0;

  for (auto i = token.index() - token.column() + 1; i < text.size(); i++) {
    if (text[i] == '\n') {
      break;
    }
    count++;
  }

  return text.substr((token.index() - token.column() + 1), count);
}

// TODO: Add count assign(+=/-=/*=/ /= %=)
void SLang::Lexer::tokenize() {
  curr_ch = text[index];

  while (curr_ch != END_OF_STREAM) {
    switch (curr_ch) {
    case '\t':
    case ' ':
    case '\r':
      break;
    case '\n':
      next();
      continue;
    case ',':
      addToken(TokenType::COMMA);
      break;
    case '.':
      if (is_digit(peek())) {
        read_number();
      } else {
        addToken(TokenType::DOT);
      }
      break;
    case ';':
      addToken(TokenType::SEMICOLON);
      break;
    case '?':
      addToken(TokenType::QUEST);
      break;
    case '%':
      addToken(TokenType::PERCENT);
      break;
    case ':':
      addToken(TokenType::COLON);
      break;
    case '>':
      if (peek() == '=') {
        addToken(TokenType::GE);
        next();
      } else {
        addToken(TokenType::GR);
      }
      break;
    case '<':
      if (peek() == '=') {
        addToken(TokenType::LE);
        next();
      } else {
        addToken(TokenType::LS);
      }
      break;
    case '=':
      if (peek() == '=') {
        addToken(TokenType::EQ);
        next();
      } else {
        addToken(TokenType::SET);
      }
      break;
    case '!':
      if (peek() == '=') {
        addToken(TokenType::NOT_EQ);
        next();
      } else {
        addToken(TokenType::NOT);
      }
      break;
    case '(':
      addToken(TokenType::BR_BEGIN);
      break;
    case ')':
      addToken(TokenType::BR_END);
      break;
    case '{':
      addToken(TokenType::BLK_BEGIN);
      break;
    case '}':
      addToken(TokenType::BLK_END);
      break;
    case '+':
      if (peek() == '+') {
        addToken(TokenType::INCREMENT);
        next();
      } else {
        addToken(TokenType::PLUS);
      }
      break;
    case '-':
      if (peek() == '-') {
        addToken(TokenType::DECREMENT);
        next();
      } else {
        addToken(TokenType::MINUS);
      }
      break;
    case '*':
      addToken(TokenType::STAR);
      break;
    case '/':
      if (peek() == '/') {
        pass_comment();
      } else if (peek() == '*') {
        pass_multi_comment();
      } else {
        addToken(TokenType::SLASH);
      }
      break;
    case '[':
      if (peek() == '[') {
        next();
        read_multi_string();
      } else {
        addToken(TokenType::BRACKET_BEGIN);
      }
      break;
    case ']':
      addToken(TokenType::BRACKET_END);
      break;
    case '\"':
      read_string();
      break;

    default:
      if (is_digit(curr_ch)) {
        read_number();
      } else if (is_alpha(curr_ch) || curr_ch == '_') {
        read_identifier();
      } else {
        econ->add_msg(MessageType::ERR, line, column, get_line(),
                      ERROR_UNEXPECTED_CHAR, curr_ch);
        panic();
      }
    }

    next();
  }

  addToken(TokenType::_EOF);

  if (!econ->empty()) {
    panic();
  }

  econ->clear();
}
