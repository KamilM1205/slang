#include "parser.hpp"
#include <gtest/gtest.h>

TEST(PARSER_TEST, test_parser) {
  Parser parser("import \"aabb\"; var a = 52;");
  parser.parse();
}
