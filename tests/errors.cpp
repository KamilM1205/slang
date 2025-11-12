#include "errors.hpp"
#include "error.hpp"
#include <gtest/gtest.h>

TEST(ErrorTest, ErrorOutput) {
  ErrorContainer *econ = ErrorContainer::get_instance();

  econ->add_error(ErrorType::ERR, ERROR_UNEXPECTED_EOF);
  econ->clear();
}

TEST(ErrorTest, ErrorFormatter) {
  std::string expected =
      R"(Error: at [112; 3] message here
    112 | abbcd
            ^
)";

  ErrorContainer *econ = ErrorContainer::get_instance();

  econ->add_error(ErrorType::ERR, 112, 3, "abbcd", "message here");

  std::cout << "Result:" << std::endl << econ->to_string();
  ASSERT_EQ(expected, econ->to_string());

  econ->clear();
}
