#ifndef ERROR_HPP
#define ERROR_HPP

#include <optional>
#include <string>
#include <vector>

enum class ErrorType {
  INFO,
  WARN,
  ERR,
};

// TODO: In feature add path to file where error was occured
class Error {
private:
  std::optional<size_t> line;
  std::optional<size_t> column;
  std::optional<std::string> line_src;
  std::string message;
  ErrorType type;

public:
  Error() = delete;
  Error(ErrorType type, std::string message) : type(type), message(message) {}
  Error(ErrorType type, size_t line, size_t column, std::string message)
      : type(type), line(std::optional(line)), column(std::optional(column)),
        message(message) {}
  Error(ErrorType type, size_t line, size_t column, std::string line_src,
        std::string message)
      : type(type), line(std::optional(line)), column(std::optional(column)),
        line_src(std::optional(line_src)), message(message) {}

  std::optional<size_t> get_line();
  std::optional<size_t> get_column();
  std::optional<std::string> get_line_src();
  std::string get_message();
  ErrorType get_error_type();
};

class ErrorContainer {
private:
  ErrorContainer() {};

  static ErrorContainer *instance;

  std::vector<Error> errors;

public:
  void add_error(ErrorType type, size_t line, size_t column,
                 std::string line_src, std::string message);
  void add_error(ErrorType type, size_t line, size_t column,
                 std::string message);
  void add_error(ErrorType type, std::string message);

  static ErrorContainer *get_instance();

  auto to_string() -> std::string;
  void clear();
  bool empty() const;

  ErrorContainer(ErrorContainer &e) = delete;
  ErrorContainer &operator=(const ErrorContainer &e) = delete;
};

#endif // _ERROR_HPP
