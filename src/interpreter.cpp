#include "interpreter.hpp"
#include "ast.hpp"
#include "benchmark.hpp"
#include "value.hpp"
#include <cassert>
#include <cstddef>
#include <memory>
#include <ranges>
#include <stdexcept>
#include <variant>

auto SLang::ReturnException::get_value() -> std::optional<Value> {
  return value;
}

SLang::Value SLang::Function::operator()(Interpreter &interpreter, Args args) {
  SLAssert(this->args.size() == args.size(),
           std::format("Function: {} accepts {} arguments - given {}.", name,
                       this->args.size(), args.size())
               .c_str());
  if (std::holds_alternative<FnP>(body)) {
    return std::get<FnP>(body)(interpreter.get_global_env(), args);
  } else {
    auto block = std::ref(std::get<AST::BlockStmt>(body));
    Value ret_value{ret_type};
    Environment &env = interpreter.get_global_env();

    env.push_env();

    for (size_t i = 0; i < args.size(); i++) {
      env.add_var(this->args[i].first, args[i].get_raw_value());
    }

    for (auto &stmt : block.get().stmts()) {
      try {
        stmt->accept(interpreter);
      } catch (ReturnException ret) {
        if (ret.get_value().has_value()) {
          ret_value = ret.get_value().value();
        } else {
          ret_value = Value();
        }
        SLAssert(ret_value.get_type() == ret_type,
                 std::format("Error: expected return with type: {}, found: {}",
                             Value::type_to_string(ret_type),
                             ret_value.type_to_string()));
        env.pop_env();
        return std::move(ret_value);
      }
    }

    env.pop_env();
  }

  return Value();
}

auto SLang::Function::get_args() -> SLang::ArgsTypes & { return args; }

auto SLang::Function::get_name() -> std::string & { return name; }

void SLang::Environment::add_var(std::string name, Value &&value) {
  if (stack.back().vars.contains(name)) {
    throw std::runtime_error(
        std::format("Error: variable: '{}' already exists.", name));
  }

  stack.back().vars[name] = std::make_shared<Value>(std::move(value));
}

void SLang::Environment::set_var(std::string name, Value &&value) {
  for (auto &env : stack) {
    if (env.vars.contains(name)) {
      *env.vars[name] = std::move(value);
      return;
    }
  }

  throw std::runtime_error(std::format("Error: variable: {} not found.", name));
}

auto SLang::Environment::get_var(std::string name) -> Value {
  for (auto &env : stack | std::views::reverse) {
    if (env.vars.contains(name)) {
      return env.vars[name];
    }
  }

  throw std::runtime_error(std::format("Variable: '{}' not found.", name));
}

void SLang::Environment::add_fn(Function fn) {
  std::string name{fn.get_name()};
  if (stack.back().funcs.contains(name)) {
    throw std::runtime_error(
        std::format("Error: variable {} already exists.", fn.get_name()));
  }
  stack.back().funcs[name] = std::make_shared<Function>(std::move(fn));
}

SLang::Value SLang::Environment::call_fn(Interpreter &interpreter,
                                         std::string name, Args args) {
  for (auto &env : stack | std::views::reverse) {
    if (env.funcs.contains(name)) {
      return (*env.funcs[name])(interpreter, args);
    }
  }

  throw std::runtime_error(std::format("Function: {} not found.", name));
}

void SLang::Environment::push_env() { stack.push_back(Env()); }

void SLang::Environment::pop_env() { stack.pop_back(); }

void SLang::Interpreter::push(Value value) {
  stack.push_back(std::move(value));
}

auto SLang::Interpreter::pop() -> Value {
  Value value = std::move(stack.back());
  stack.pop_back();
  return value;
}

void SLang::Interpreter::visit(SLang::AST::Expr &expr) {}

void SLang::Interpreter::visit(SLang::AST::ValueExpr &expr) {
  if (expr.value().get_type() == TokenType::IDENTIFIER) {
    push(env.get_var(expr.value().get_value()));
  } else {
    push(Value(expr.value()));
  }
}

void SLang::Interpreter::visit(SLang::AST::FunCallExpr &expr) {
  std::vector<Value> args;

  for (auto &arg : expr.args()) {
    arg->accept(*this);
    args.push_back(pop().get_raw_value());
  }

  push(env.call_fn(*this, expr.ident().get_value(), std::move(args)));
}

void SLang::Interpreter::visit(SLang::AST::GroupingExpr &expr) {
  expr.expr()->accept(*this);
}

void SLang::Interpreter::visit(SLang::AST::UnaryExpr &expr) {
  expr.right()->accept(*this);

  Value value = pop();

  switch (expr.op().get_type()) {
  case TokenType::INCREMENT:
    value++;
    break;

  case TokenType::DECREMENT:
    value--;
    break;

  case TokenType::PLUS:
    break;

  case TokenType::MINUS:
    value = value * Value(-1);
    break;

  case TokenType::NOT:
    value = value != Value(true);
    break;
  default:
    throw std::runtime_error("Expected unary operation.");
    break;
  }

  push(value);
}

void SLang::Interpreter::visit(SLang::AST::BinaryExpr &expr) {
  expr.left()->accept(*this);
  Value left = pop();

  expr.right()->accept(*this);
  Value right = pop();

  switch (expr.op().get_type()) {
  case TokenType::PLUS:
    push(left + right);
    break;
  case TokenType::MINUS:
    push(left - right);
    break;
  case TokenType::STAR:
    push(left * right);
    break;
  case TokenType::SLASH:
    push(left / right);
    break;
  case TokenType::PERCENT:
    push(left % right);
    break;
  case TokenType::LE:
    push(left <= right);
    break;
  case TokenType::LS:
    push(left < right);
    break;
  case TokenType::GE:
    push(left >= right);
    break;
  case TokenType::GR:
    push(left > right);
    break;
  case TokenType::EQ:
    push(left == right);
    break;
  case TokenType::NOT_EQ:
    push(left != right);
    break;
  case TokenType::AND:
    if (left == Value(true) && right == Value(true)) {
      push(true);
    } else {
      push(false);
    }
    break;
  case TokenType::OR:
    if (left == Value(true) || right == Value(true)) {
      push(true);
    } else {
      push(false);
    }
    break;
  default:
    throw std::runtime_error("Unexpected binary operation.");
  }
}

void SLang::Interpreter::visit(SLang::AST::TernaryExpr &expr) {
  Value cond(ValueType::BOOL);

  expr.cond()->accept(*this);
  cond = pop();

  if (cond == true) {
    expr.expr_true()->accept(*this);
  } else {
    expr.expr_false()->accept(*this);
  }
}

void SLang::Interpreter::visit(SLang::AST::AssignExpr &expr) {
  expr.right()->accept(*this);

  env.set_var(expr.ident().get_value(), pop());
}

void SLang::Interpreter::visit(SLang::AST::DefineExpr &expr) {
  if (expr.expr().has_value()) {
    expr.expr().value()->accept(*this);

    Value value = pop();

    if (expr.type().get_type() != TokenType::AUTO) {
      if (Value::from_token_type(expr.type().get_type()) != value.get_type()) {
        throw std::runtime_error(
            "Error: expression type is not match to definition type.");
      }
    }

    env.add_var(expr.ident().get_value(), std::move(value));
    return;
  }

  env.add_var(expr.ident().get_value(),
              Value(Value::from_token_type(expr.type().get_type())));
}

void SLang::Interpreter::visit(SLang::AST::ImportStmt &stmt) {}

void SLang::Interpreter::visit(SLang::AST::ArgDefineExpr &expr) {}

void SLang::Interpreter::visit(SLang::AST::BlockStmt &stmt) {
  env.push_env();

  for (const auto &stmt : stmt.stmts()) {
    try {
      stmt->accept(*this);
      stack.clear(); // That's need because we have stmt like increment that
                     // pushes value to stack and no one take it.
    } catch (ReturnException ret) {
      env.pop_env();
      stack.clear();
      throw ReturnException(ret);
    }
  }

  env.pop_env();
}

void SLang::Interpreter::visit(SLang::AST::FnDefineStmt &stmt) {
  std::string fn_name = stmt.ident().get_value();
  ValueType ret_type =
      stmt.ret_type().has_value()
          ? Value::from_token_type(stmt.ret_type().value().get_type())
          : ValueType::NONE;
  ArgsTypes args = {};

  for (auto &arg : stmt.args()) {
    args.push_back(std::pair(arg.ident().get_value(),
                             Value::from_token_type(arg.type().get_type())));
  }

  env.add_fn(
      Function(fn_name, std::move(args), std::move(stmt.block()), ret_type));
}

void SLang::Interpreter::visit(SLang::AST::ReturnStmt &stmt) {
  if (stmt.expr().has_value()) {
    stmt.expr().value()->accept(*this);
    throw ReturnException(std::move(pop().get_raw_value()));
  } else {
    throw ReturnException();
  }
}

void SLang::Interpreter::visit(SLang::AST::IfStmt &stmt) {
  Value cond_value(ValueType::BOOL);

  stmt.cond()->accept(*this);
  cond_value = std::move(pop()).get_raw_value();
  SLAssert(cond_value.get_type() == ValueType::BOOL,
           "condition must be boolean type.");

  if (cond_value == true) {
    stmt.true_block().accept(*this);
    return;
  }

  for (auto &elif : stmt.elif_block()) {
    elif->cond()->accept(*this);
    cond_value = std::move(pop()).get_raw_value();
    SLAssert(cond_value.get_type() == ValueType::BOOL,
             "condition must be boolean type.");
    if (cond_value == true) {
      elif->true_block().accept(*this);
      return;
    }
  }

  if (stmt.else_block().has_value()) {
    stmt.else_block().value().accept(*this);
  }
}

void SLang::Interpreter::visit(SLang::AST::WhileStmt &stmt) {
  Value cond_value(ValueType::BOOL);

  stmt.cond()->accept(*this);
  cond_value = std::move(pop()).get_raw_value();
  SLAssert(cond_value.get_type() == ValueType::BOOL,
           "condition must be boolean type.");

  while (cond_value == true) {
    stmt.block().accept(*this);
    stmt.cond()->accept(*this);
    cond_value = std::move(pop()).get_raw_value();
  }
}

void SLang::Interpreter::visit(SLang::AST::ForStmt &stmt) {
  Value cond_value(ValueType::BOOL);

  env.push_env();
  stmt.init()->accept(*this);
  stmt.cond()->accept(*this);
  cond_value = std::move(pop()).get_raw_value();
  assert(cond_value.get_type() == ValueType::BOOL &&
         "condition must be boolean type.");

  while (cond_value == true) {
    stmt.block().accept(*this);
    stmt.step()->accept(*this);
    pop(); // Because after step execution we have result value in stack
    stmt.cond()->accept(*this);
    cond_value = std::move(pop()).get_raw_value();
  }

  env.pop_env();
}

void SLang::Interpreter::visit(SLang::AST::InterfaceFnDefineStmt &stmt) {}

void SLang::Interpreter::visit(SLang::AST::InterfaceStmt &stmt) {}

void SLang::Interpreter::visit(SLang::AST::ClassStmt &stmt) {}

void SLang::Interpreter::visit(SLang::AST::ClassField &stmt) {}

void SLang::Interpreter::visit(SLang::AST::ClassFnDefineStmt &stmt) {}

void SLang::Interpreter::visit(SLang::AST::ClassMemberGetter &stmt) {}

void SLang::Interpreter::visit(SLang::AST::ClassMemberSetter &stmt) {}

void SLang::Interpreter::visit(SLang::AST::ClassMemberCall &stmt) {}

void SLang::Interpreter::visit(SLang::AST::ClassMemberAccess &stmt) {}

void SLang::Interpreter::visit(SLang::AST::ExprStmt &stmt) {
  stmt.expr()->accept(*this);
}

void SLang::Interpreter::visit(SLang::AST::VoidStmt &stmt) {}

void SLang::Interpreter::execute() {
  SLANG_TIME(Interpreter);
  if (tree == nullptr) {
    throw std::runtime_error("AST not passed to interpreter.");
  }

  auto stmts = tree->get_stmts();
  for (size_t i = 0; i < stmts->size(); i++) {
    stmts->at(i)->accept(*this);
    stack.clear();
  }
}

auto SLang::Interpreter::get_global_env() -> Environment & { return env; }
