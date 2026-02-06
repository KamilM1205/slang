/**
 * @file error.hpp
 * @brief Хранит строки ошибок
 * @author Kamil Meftahutdinov
 * @date 2026
 */
#include "errors.hpp"

/* Lexer errors */
const char *ERROR_LINE_OVERFLOW = "The line counter has overflowed.";
const char *ERROR_INDEX_OVERFLOW = "The source index has overflowed.";
const char *ERROR_NUMBER_EXPECTED = "Expected digit, found letter.";
const char *ERROR_UNEXPECTED_EOF = "Unexpected end of file.";
const char *ERROR_UNEXPECTED_EOL = "Unexpected end of line.";
const char *ERROR_EXPECTED_QUOTE = "Expected quote, found end of line.";
const char *ERROR_UNEXPECTED_ESC = "Unexpected escape character.";
const char *ERROR_EXPECTED_BIN = "Expected binary digit.";
const char *ERROR_EXPECTED_HEX = "Expected hexadecimal digit.";
const char *ERROR_UNCLOSED_MULTISTRING = "Unclosed multiline string.";
const char *ERROR_UNCLOSED_MULTICOMMENT = "Unclosed multiline comment.";
const char *ERROR_UNEXPECTED_CHAR = "Unexpected character: {}";

/* Parser errors */
const char *ERROR_TOKENS_END = "Expected new token, found last token.";
const char *ERROR_UNEXPECTED_TOKEN = "Unexpected token found: {}";
const char *ERROR_EXPECTED_TOKEN = "Expected token: {}. Found: {}.";
const char *ERROR_EXPRESSION = "Incorrect expression.";
const char *ERROR_UNCLOSED_BLOCK = "Unclosed block at line: {}.";
const char *ERROR_INTERFACE_METHOD_PRIVATE =
    "Interface method can't be private.";
const char *ERROR_ALREADY_BEGIN =
    "Can't get pervious token because index on begin of tokens list.";
