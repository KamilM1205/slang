#include "error.hpp"
#include <iomanip>
#include <optional>
#include <ostream>
#include <sstream>
#include <string>

ErrorContainer *ErrorContainer::instance = nullptr;

std::optional<size_t> Error::get_line() { return line; }

std::optional<size_t> Error::get_column() { return column; }

std::optional<std::string> Error::get_line_src() { return line_src; }

std::string Error::get_message() { return message; }

ErrorType Error::get_error_type() { return type; }

ErrorContainer *ErrorContainer::get_instance() {
  if (instance == nullptr) {
    instance = new ErrorContainer();
  }

  return instance;
}

void ErrorContainer::add_error(ErrorType type, std::string message) {
  errors.push_back(Error(type, message));
}

void ErrorContainer::add_error(ErrorType type, size_t line, size_t column,
                               std::string message) {
  errors.push_back(Error(type, line, column, message));
}

void ErrorContainer::add_error(ErrorType type, size_t line, size_t column,
                               std::string line_src, std::string message) {
  errors.push_back(Error(type, line, column, line_src, message));
}

std::string ErrorContainer::to_string() {
  std::stringstream ss;

  for (auto error : errors) {
    // Error level
    switch (error.get_error_type()) {
    case ErrorType::INFO:
      ss << "Info: ";
      break;
    case ErrorType::WARN:
      ss << "Warning: ";
      break;
    case ErrorType::ERR:
      ss << "Error: ";
      break;
    }

    // If has column and line output they
    if (error.get_line().has_value() && error.get_column().has_value()) {
      ss << "at [" << error.get_line().value() << "; "
         << error.get_column().value() << "] ";
    }

    ss << error.get_message();

    if (error.get_line_src().has_value()) {
      ss << std::endl;
      ss << std::setw(4) << ' ' << error.get_line().value() << " | "
         << error.get_line_src().value() << std::endl;
      ss << std::setw(4 + std::to_string(error.get_line().value()).size() + 3 +
                      error.get_column().value())
         << '^';
    }

    ss << std::endl;
  }

  return ss.str();
}

void ErrorContainer::clear() { errors.clear(); }

bool ErrorContainer::empty() const { return errors.empty(); }
