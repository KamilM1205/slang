#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP

#include "ast.hpp"
#include "value.hpp"
#include <exception>
#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

namespace SLang {

using Args = std::vector<Value>;
using ArgsTypes = std::vector<std::pair<std::string, ValueType>>;
class Function;
class Environment;
class Interpreter;

class ReturnException : std::exception {
private:
  Value value;

public:
  ReturnException(Value value) : value(value) {}
  auto get_value() -> Value;
};

class Function {
private:
  using FnP = std::function<Value(Environment &env, Args)>;
  using FnBody = std::variant<AST::BlockStmt, FnP>;

  std::string name;
  ArgsTypes args;
  FnBody body;
  ValueType ret_type;

public:
  Function() = delete;
  Function(std::string name, ArgsTypes args, FnBody body)
      : name(name), args(std::move(args)), body(std::move(body)),
        ret_type(ValueType::NONE) {}
  Function(std::string name, ArgsTypes args, FnBody body, ValueType ret_type)
      : name(name), args(std::move(args)), body(std::move(body)),
        ret_type(ret_type) {}

  auto get_name() -> std::string &;
  auto get_args() -> ArgsTypes &;

  Value operator()(Interpreter &interpreter, Args args);
};

class Environment {
private:
  struct Env {
    std::unordered_map<std::string, std::shared_ptr<Value>> vars;
    std::unordered_map<std::string, std::shared_ptr<Function>> funcs;
  };

  std::vector<Env> stack;

public:
  Environment() : stack({Env()}) {}

  void add_var(std::string name, Value &&value);
  void set_var(std::string name, Value &&value);
  auto get_var(std::string name) -> Value;

  void add_fn(Function fn);
  Value call_fn(Interpreter &interpreter, std::string name, Args args);

  void push_env();
  void pop_env();
};

class Interpreter : public AST::IExprVisitor {
private:
  AST::ASTree *tree;
  Environment env;
  std::vector<Value> stack;

public:
  Interpreter(AST::ASTree *tree) : tree(tree) {}
  Interpreter() = default;

  void execute();
  auto get_global_env() -> Environment &;
  void set_tree(AST::ASTree *tree) { this->tree = tree; }

  void push(Value value);
  auto pop() -> Value;

protected:
  virtual void visit(AST::Expr &expr) override;
  virtual void visit(AST::ValueExpr &expr) override;
  virtual void visit(AST::FunCallExpr &expr) override;
  virtual void visit(AST::GroupingExpr &expr) override;
  virtual void visit(AST::UnaryExpr &expr) override;
  virtual void visit(AST::BinaryExpr &expr) override;
  virtual void visit(AST::TernaryExpr &expr) override;
  virtual void visit(AST::AssignExpr &expr) override;
  virtual void visit(AST::DefineExpr &expr) override;
  virtual void visit(AST::ImportStmt &stmt) override;
  virtual void visit(AST::ArgDefineExpr &expr) override;
  virtual void visit(AST::BlockStmt &stmt) override;
  virtual void visit(AST::FnDefineStmt &stmt) override;
  virtual void visit(AST::ReturnStmt &stmt) override;
  virtual void visit(AST::IfStmt &stmt) override;
  virtual void visit(AST::WhileStmt &stmt) override;
  virtual void visit(AST::ForStmt &stmt) override;
  virtual void visit(AST::InterfaceFnDefineStmt &stmt) override;
  virtual void visit(AST::InterfaceStmt &stmt) override;
  virtual void visit(AST::ClassStmt &stmt) override;
  virtual void visit(AST::ClassField &stmt) override;
  virtual void visit(AST::ClassFnDefineStmt &stmt) override;
  virtual void visit(AST::ClassMemberGetter &stmt) override;
  virtual void visit(AST::ClassMemberSetter &stmt) override;
  virtual void visit(AST::ClassMemberCall &stmt) override;
  virtual void visit(AST::ClassMemberAccess &stmt) override;
  virtual void visit(AST::ExprStmt &stmt) override;
  virtual void visit(AST::VoidStmt &stmt) override;
};
} // namespace SLang

#endif // !INTERPRETER_HPP
