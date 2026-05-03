/**
 * @file error.hpp
 * @brief Хранит объявление констант строк ошибок
 * @author Kamil Meftahutdinov
 * @date 2026
 */
#ifndef ERRORS_HPP
#define ERRORS_HPP

/* Lexer errors */
extern const char *ERROR_LINE_OVERFLOW;
extern const char *ERROR_INDEX_OVERFLOW;
extern const char *ERROR_NUMBER_EXPECTED;
extern const char *ERROR_UNEXPECTED_EOF;
extern const char *ERROR_UNEXPECTED_EOL;
extern const char *ERROR_EXPECTED_QUOTE;
extern const char *ERROR_UNEXPECTED_ESC;
extern const char *ERROR_EXPECTED_BIN;
extern const char *ERROR_EXPECTED_HEX;
extern const char *ERROR_UNCLOSED_MULTISTRING;
extern const char *ERROR_UNCLOSED_MULTICOMMENT;
extern const char *ERROR_UNEXPECTED_CHAR;

/* Parser errors */
extern const char *ERROR_TOKENS_END;
extern const char *ERROR_UNEXPECTED_TOKEN;
extern const char *ERROR_EXPECTED_TOKEN;
extern const char *ERROR_EXPRESSION;
extern const char *ERROR_UNCLOSED_BLOCK;
extern const char *ERROR_ALREADY_BEGIN;
extern const char *ERROR_INTERFACE_METHOD_PRIVATE;
extern const char *ERROR_EXPECTED_MEMBER_ACCESS;
extern const char *ERROR_EXPECTED_MEMBER_OP;

#endif // !ERRORS_HPP
