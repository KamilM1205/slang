#ifndef ERROR_HPP
#define ERROR_HPP

#include "token.hpp"
#include <format>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

enum class MessageType {
  INFO,
  WARN,
  ERR,
};

// TODO: In feature add path to file where error was occured
class Message {
private:
  std::optional<size_t> line;
  std::optional<size_t> column;
  std::optional<std::string> line_src;
  std::string message;
  MessageType type;

public:
  Message() = delete;
  Message(MessageType type, std::string message)
      : type(type), message(message) {}
  Message(MessageType type, size_t line, size_t column, std::string message)
      : type(type), line(line), column(column), message(message) {}
  Message(MessageType type, size_t line, size_t column, std::string line_src,
          std::string message)
      : type(type), line(line), column(column), line_src(line_src),
        message(message) {}

  std::optional<size_t> get_line();
  std::optional<size_t> get_column();
  std::optional<std::string> get_line_src();
  std::string get_message();
  MessageType get_msg_type();
};

class MessageContainer {
private:
  MessageContainer() {};

  static MessageContainer *instance;

  std::vector<Message> errors;

  std::string *src = nullptr;

  auto get_line(const Token &token) -> std::string;

  template <typename... Args>
  std::string make_msg(std::string msg, Args... args) {
    size_t args_count = sizeof...(args);

    if (src == nullptr) {
      throw std::runtime_error("Source text not set for MessageContainer.");
    }

    if (args_count != 0) {
      msg = std::vformat(msg, std::make_format_args(args...));
    }

    return msg;
  }

public:
  void add_msg(MessageType type, size_t line, size_t column,
               std::string line_src, std::string message);
  void add_msg(MessageType type, std::string message);

  void add_msg(MessageType type, size_t line, size_t column,
               std::string message);

  template <typename... Args>
  void add_msg(MessageType type, size_t line, size_t column, std::string msg,
               Args... args) {
    errors.push_back(Message(type, line, column, make_msg(msg, args...)));
  }

  template <typename... Args>
  void add_msg(MessageType type, size_t line, size_t column,
               std::string line_src, std::string msg, Args... args) {
    errors.push_back(
        Message(type, line, column, line_src, make_msg(msg, args...)));
  }

  template <typename... Args>
  void add_msg(MessageType type, Token token, std::string msg, Args... args) {
    errors.push_back(Message(type, token.line(), token.column(),
                             get_line(token), make_msg(msg, args...)));
  }

  template <typename... Args>
  void add_msg(MessageType type, std::string msg, Args... args) {
    errors.push_back(Message(type, make_msg(msg, args...)));
  }

  static MessageContainer *get_instance();

  auto to_string() -> std::string;
  void set_src(std::string *src);
  void clear();
  bool empty() const;

  MessageContainer(MessageContainer &e) = delete;
  MessageContainer &operator=(const MessageContainer &e) = delete;
};

#endif // _ERROR_HPP
