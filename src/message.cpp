/**
 * @file error.hpp
 * @brief Хранит тип данных ошибки и класс-синглтон контейнер для ошибок.
 * @details Хранит тип данных Error, который нужен для сохранения сообщения и
 * информации о том, где произошла ошибка. Также есть класс MessageContainer
 * который реализует паттерн синглтон и нужен для сохранения пула ошибок и
 * вывода этих ошибок в консоль.
 * @author Kamil Meftahutdinov
 * @date 2026
 */

#include "message.hpp"
#include <iomanip>
#include <optional>
#include <ostream>
#include <sstream>
#include <string>

/*
 * @brief Глобальный экземпляр контейнера ошибок.
 */
MessageContainer *MessageContainer::instance = nullptr;

std::optional<size_t> Message::get_line() { return line; }

std::optional<size_t> Message::get_column() { return column; }

std::optional<std::string> Message::get_line_src() { return line_src; }

std::string Message::get_message() { return message; }

MessageType Message::get_msg_type() { return type; }

MessageContainer *MessageContainer::get_instance() {
  if (instance == nullptr) {
    instance = new MessageContainer();
  }

  return instance;
}

void MessageContainer::add_msg(MessageType type, std::string message) {
  errors.push_back(Message(type, message));
}

void MessageContainer::add_msg(MessageType type, size_t line, size_t column,
                               std::string message) {
  errors.push_back(Message(type, line, column, message));
}

void MessageContainer::add_msg(MessageType type, size_t line, size_t column,
                               std::string line_src, std::string message) {
  errors.push_back(Message(type, line, column, line_src, message));
}

auto MessageContainer::get_line(const Token &token) -> std::string {
  size_t count = 0;

  for (auto i = token.index() - token.column() + 1; i < src->size(); i++) {
    if (src->at(i) == '\n') {
      break;
    }
    count++;
  }

  return src->substr((token.index() - token.column() + 1), count);
}

/*
 * @brief Трансляция массива ошибок в цельное сообщение.
 * Транслирует массив ошибок в строку хранящее цельное и отформатированное
 * сообщение
 */
std::string MessageContainer::to_string() {
  std::stringstream ss;

  for (auto error : errors) {
    // Error level
    switch (error.get_msg_type()) {
    case MessageType::INFO:
      ss << "Info: ";
      break;
    case MessageType::WARN:
      ss << "Warning: ";
      break;
    case MessageType::ERR:
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

void MessageContainer::set_src(std::string *src) { this->src = src; }

void MessageContainer::clear() { errors.clear(); }

bool MessageContainer::empty() const { return errors.empty(); }
