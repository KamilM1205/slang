/**
 * @file error.hpp
 * @brief Хранит строки ошибок
 * @author Kamil Meftahutdinov
 * @date 2026
 */
#include "errors.hpp"

/* Lexer errors */
const char *SLang::ERROR_LINE_OVERFLOW = "The line counter has overflowed.";
const char *SLang::ERROR_INDEX_OVERFLOW = "The source index has overflowed.";
const char *SLang::ERROR_NUMBER_EXPECTED = "Expected digit, found letter.";
const char *SLang::ERROR_UNEXPECTED_EOF = "Unexpected end of file.";
const char *SLang::ERROR_UNEXPECTED_EOL = "Unexpected end of line.";
const char *SLang::ERROR_EXPECTED_QUOTE = "Expected quote, found end of line.";
const char *SLang::ERROR_UNEXPECTED_ESC = "Unexpected escape character.";
const char *SLang::ERROR_EXPECTED_BIN = "Expected binary digit.";
const char *SLang::ERROR_EXPECTED_HEX = "Expected hexadecimal digit.";
const char *SLang::ERROR_UNCLOSED_MULTISTRING = "Unclosed multiline string.";
const char *SLang::ERROR_UNCLOSED_MULTICOMMENT = "Unclosed multiline comment.";
const char *SLang::ERROR_UNEXPECTED_CHAR = "Unexpected character: {}";

/* Parser errors */
const char *SLang::ERROR_TOKENS_END = "Expected new token, found last token.";
const char *SLang::ERROR_UNEXPECTED_TOKEN = "Unexpected token found: {}";
const char *SLang::ERROR_EXPECTED_TOKEN = "Expected token: {}. Found: {}.";
const char *SLang::ERROR_EXPRESSION = "Incorrect expression.";
const char *SLang::ERROR_UNCLOSED_BLOCK = "Unclosed block at line: {}.";
const char *SLang::ERROR_INTERFACE_METHOD_PRIVATE =
    "Interface method can't be private.";
const char *SLang::ERROR_ALREADY_BEGIN =
    "Can't get pervious token because index on begin of tokens list.";
const char *SLang::ERROR_EXPECTED_MEMBER_ACCESS = "Expected member access.";
const char *SLang::ERROR_EXPECTED_MEMBER_OP =
    "Expected class member assign or method call.";
const char *SLang::ERROR_VARIABLE_DEFINITION =
    "Expected variable type or expression for type detection.";
