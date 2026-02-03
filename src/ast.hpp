#ifndef AST_HPP
#define AST_HPP

#include "lexer.hpp"
#include "utils.hpp"
#include <memory>
#include <optional>
#include <source_location>
#include <sstream>
#include <string>
#include <vector>

// TODO: Add documentation

namespace AST {

class Expr;
class ValueExpr;
class FunCallExpr;
class GroupingExpr;
class UnaryExpr;
class BinaryExpr;
class TernaryExpr;
class AssignExpr;
class DefineExpr;
class ImportStmt;
class ArgDefineExpr;
class BlockStmt;
class FnDefineStmt;
class ReturnStmt;
class IfStmt;
class WhileStmt;
class ForStmt;
class InterfaceFnDefineStmt;
class InterfaceStmt;
class ClassStmt;
class ClassField;
class ClassFnDefineStmt;
class ExprStmt;
class VoidStmt; // Need to shut up warning about empty return from "parse_stmt"

class IExprVisitor {
public:
  virtual void visit(Expr &expr) = 0;
  virtual void visit(ValueExpr &expr) = 0;
  virtual void visit(FunCallExpr &expr) = 0;
  virtual void visit(GroupingExpr &expr) = 0;
  virtual void visit(UnaryExpr &expr) = 0;
  virtual void visit(BinaryExpr &expr) = 0;
  virtual void visit(TernaryExpr &expr) = 0;
  virtual void visit(AssignExpr &expr) = 0;
  virtual void visit(DefineExpr &expr) = 0;
  virtual void visit(ImportStmt &stmt) = 0;
  virtual void visit(ArgDefineExpr &expr) = 0;
  virtual void visit(BlockStmt &stmt) = 0;
  virtual void visit(FnDefineStmt &stmt) = 0;
  virtual void visit(ReturnStmt &stmt) = 0;
  virtual void visit(IfStmt &stmt) = 0;
  virtual void visit(WhileStmt &stmt) = 0;
  virtual void visit(ForStmt &stmt) = 0;
  virtual void visit(InterfaceFnDefineStmt &stmt) = 0;
  virtual void visit(InterfaceStmt &stmt) = 0;
  virtual void visit(ClassStmt &stmt) = 0;
  virtual void visit(ClassField &stmt) = 0;
  virtual void visit(ClassFnDefineStmt &stmt) = 0;
  virtual void visit(ExprStmt &stmt) = 0;
  virtual void visit(VoidStmt &stmt) = 0;
};

class ExprPrinter : public IExprVisitor {
private:
  std::stringstream ss;
  size_t indent_level = 0;

  void tab();
  void indent();
  void
  untab(const std::source_location &location = std::source_location::current());
  void newline();

public:
  void visit(Expr &expr) override;
  void visit(ValueExpr &expr) override;
  void visit(FunCallExpr &expr) override;
  void visit(GroupingExpr &expr) override;
  void visit(UnaryExpr &expr) override;
  void visit(BinaryExpr &expr) override;
  void visit(TernaryExpr &expr) override;
  void visit(AssignExpr &expr) override;
  void visit(DefineExpr &expr) override;
  void visit(ImportStmt &stmt) override;
  void visit(ArgDefineExpr &expr) override;
  void visit(BlockStmt &stmt) override;
  void visit(FnDefineStmt &stmt) override;
  void visit(ReturnStmt &stmt) override;
  void visit(IfStmt &stmt) override;
  void visit(WhileStmt &stmt) override;
  void visit(ForStmt &stmt) override;
  void visit(InterfaceFnDefineStmt &stmt) override;
  void visit(InterfaceStmt &stmt) override;
  void visit(ClassStmt &stmt) override;
  void visit(ClassField &stmt) override;
  void visit(ClassFnDefineStmt &stmt) override;
  void visit(ExprStmt &stmt) override;
  void visit(VoidStmt &stmt) override;

  auto get_string() -> std::string;

  ExprPrinter() = default;
  ~ExprPrinter() = default;
};

class Expr {
public:
  virtual void accept(IExprVisitor &visitor) = 0;
  virtual ~Expr() = default;
};

using Expr_t = std::unique_ptr<Expr>;

class ValueExpr : public Expr {
  getter(Token, value);

public:
  ValueExpr(Token value) : _value(value) {}

  void accept(IExprVisitor &visitor) { visitor.visit(*this); }
};

class FunCallExpr : public Expr {
  getter(Token, ident);
  getter(std::vector<Expr_t>, args);

public:
  FunCallExpr(Token ident, std::vector<Expr_t> args)
      : _ident(ident), _args(std::move(args)) {}

  void accept(IExprVisitor &visitor) { visitor.visit(*this); }
};

class GroupingExpr : public Expr {
  getter(Expr_t, expr);

public:
  GroupingExpr(Expr_t expr) : _expr(std::move(expr)) {}

  void accept(IExprVisitor &visitor) { visitor.visit(*this); }
};

class UnaryExpr : public Expr {
  getter(Token, op);
  getter(Expr_t, right);

public:
  UnaryExpr(Token op, Expr_t right) : _op(op), _right(std::move(right)) {}

  void accept(IExprVisitor &visitor) { visitor.visit(*this); }
};

class BinaryExpr : public Expr {
  getter(Expr_t, left);
  getter(Token, op);
  getter(Expr_t, right);

public:
  BinaryExpr(Expr_t left, Token op, Expr_t right)
      : _left(std::move(left)), _op(op), _right(std::move(right)) {}
  void accept(IExprVisitor &visitor) { visitor.visit(*this); }
};

class TernaryExpr : public Expr {
  getter(Expr_t, cond);
  getter(Expr_t, expr_true);
  getter(Expr_t, expr_false);

public:
  TernaryExpr(Expr_t cond, Expr_t expr_true, Expr_t expr_false)
      : _cond(std::move(cond)), _expr_true(std::move(expr_true)),
        _expr_false(std::move(expr_false)) {}
  void accept(IExprVisitor &visitor) { visitor.visit(*this); }
};

class AssignExpr : public Expr {
  getter(Token, ident);
  getter(Expr_t, right);

public:
  AssignExpr(Token ident, Expr_t right)
      : _ident(ident), _right(std::move(right)) {}
  void accept(IExprVisitor &visitor) { visitor.visit(*this); }
};

class DefineExpr : public Expr {
  getter(Token, ident);
  getter(Token, type);
  getter(std::optional<Expr_t>, expr);

public:
  DefineExpr(Token ident, Token type, Expr_t expr)
      : _ident(ident), _type(type), _expr(std::move(expr)) {}

  DefineExpr(Token ident, Token type) : _ident(ident), _type(type), _expr({}) {}
  void accept(IExprVisitor &visitor) { visitor.visit(*this); }
};

class ImportStmt : public Expr {
  getter(Token, litteral);

public:
  ImportStmt(Token litteral) : _litteral(litteral) {}
  void accept(IExprVisitor &visitor) { visitor.visit(*this); }
};

class ArgDefineExpr : public Expr {
  getter(Token, ident);
  getter(Token, type);

public:
  ArgDefineExpr(Token ident, Token type) : _ident(ident), _type(type) {}

  void accept(IExprVisitor &visitor) override { visitor.visit(*this); }
};

class BlockStmt : public Expr {
  getter(std::vector<Expr_t>, stmts);

public:
  BlockStmt() = default;
  BlockStmt(std::vector<Expr_t> stmts) : _stmts(std::move(stmts)) {}
  BlockStmt(BlockStmt &&block) { _stmts = std::move(block.stmts()); };

  BlockStmt &operator=(BlockStmt &&other) {
    if (this != &other) {
      this->_stmts = std::move(other.stmts());
    }

    return *this;
  }

  void accept(IExprVisitor &visitor) override { visitor.visit(*this); }
};

class FnDefineStmt : public Expr {
  getter(Token, ident);
  getter(std::vector<ArgDefineExpr>, args);
  getter(std::optional<Token>, ret_type);
  getter(BlockStmt, block);

public:
  FnDefineStmt(Token ident, std::vector<ArgDefineExpr> args, BlockStmt block)
      : _ident(ident), _args(std::move(args)), _block(std::move(block)) {}

  FnDefineStmt(Token ident, std::vector<ArgDefineExpr> args, BlockStmt block,
               Token ret_type)
      : _ident(ident), _args(std::move(args)), _block(std::move(block)),
        _ret_type(ret_type) {}

  void accept(IExprVisitor &visitor) override { visitor.visit(*this); }
};

class ReturnStmt : public Expr {
  getter(Expr_t, expr);

public:
  ReturnStmt(Expr_t expr) : _expr(std::move(expr)) {}

  void accept(IExprVisitor &visitor) override { visitor.visit(*this); }
};

class IfStmt : public Expr {
  getter(Expr_t, cond);
  getter(BlockStmt, true_block);
  getter(std::optional<Expr_t>, elif_block);
  getter(std::optional<BlockStmt>, else_block);

public:
  IfStmt(Expr_t cond, BlockStmt true_block, std::optional<Expr_t> elif_block,
         std::optional<BlockStmt> else_block)
      : _cond(std::move(cond)), _true_block(std::move(true_block)),
        _elif_block(std::move(elif_block)), _else_block(std::move(else_block)) {
  }

  IfStmt &operator=(IfStmt &&other) noexcept {
    if (this != &other) {
      this->_cond = std::move(other._cond);
      this->_true_block = std::move(other._true_block);
      this->elif_block() = std::move(other._elif_block);
      this->else_block() = std::move(other._else_block);
    }

    return *this;
  }

  void accept(IExprVisitor &visitor) override { visitor.visit(*this); }
};

class WhileStmt : public Expr {
  getter(Expr_t, cond);
  getter(BlockStmt, block);

public:
  WhileStmt(Expr_t cond, BlockStmt block)
      : _cond(std::move(cond)), _block(std::move(block)) {}

  void accept(IExprVisitor &visitor) override { visitor.visit(*this); }
};

class ForStmt : public Expr {
  getter(Expr_t, init);
  getter(Expr_t, cond);
  getter(Expr_t, step);
  getter(BlockStmt, block);

public:
  ForStmt(Expr_t init, Expr_t cond, Expr_t step, BlockStmt block)
      : _init(std::move(init)), _cond(std::move(cond)), _step(std::move(step)),
        _block(std::move(block)) {}

  void accept(IExprVisitor &visitor) override { visitor.visit(*this); }
};

class InterfaceFnDefineStmt : public Expr {
  getter(Token, ident);
  getter(std::vector<ArgDefineExpr>, args);
  getter(std::optional<Token>, ret_type);

public:
  InterfaceFnDefineStmt(Token ident, std::vector<ArgDefineExpr> args,
                        Token ret_type)
      : _ident(ident), _args(args), _ret_type(ret_type) {}
  InterfaceFnDefineStmt(Token ident, std::vector<ArgDefineExpr> args)
      : _ident(ident), _args(args), _ret_type({}) {}

  void accept(IExprVisitor &visitor) override { visitor.visit(*this); }
};

class InterfaceStmt : public Expr {
  getter(Token, ident);
  getter(BlockStmt, block);

public:
  InterfaceStmt(Token ident, BlockStmt block)
      : _ident(ident), _block(std::move(block)) {}

  void accept(IExprVisitor &visitor) override { visitor.visit(*this); }
};

class ClassStmt : public Expr {
  getter(Token, ident);
  getter(std::optional<Token>, super);
  getter(std::optional<std::vector<Token>>, interfaces);
  getter(BlockStmt, block);

public:
  ClassStmt(Token ident, std::optional<Token> super,
            std::optional<std::vector<Token>> interfaces, BlockStmt block)
      : _ident(ident), _super(super), _interfaces(interfaces),
        _block(std::move(block)) {}

  void accept(IExprVisitor &visitor) override { visitor.visit(*this); }
};

class ClassField : public Expr {
  getter(Token, ident);
  getter(bool, is_private);
  getter(Token, type);

public:
  ClassField(Token ident, bool is_private, Token type)
      : _ident(ident), _is_private(is_private), _type(type) {}

  void accept(IExprVisitor &visitor) override { visitor.visit(*this); }
};

class ClassFnDefineStmt : public Expr {
  getter(Token, ident);
  getter(bool, is_private);
  getter(std::vector<ArgDefineExpr>, args);
  getter(std::optional<Token>, ret_type);
  getter(BlockStmt, block);

public:
  ClassFnDefineStmt(Token ident, bool is_private,
                    std::vector<ArgDefineExpr> args, Token ret_type,
                    BlockStmt block)
      : _ident(ident), _is_private(is_private), _args(args),
        _ret_type(ret_type), _block(std::move(block)) {}

  ClassFnDefineStmt(Token ident, bool is_private,
                    std::vector<ArgDefineExpr> args, BlockStmt block)
      : _ident(ident), _is_private(is_private), _args(args), _ret_type({}),
        _block(std::move(block)) {}

  void accept(IExprVisitor &visitor) override { visitor.visit(*this); }
};

class ExprStmt : public Expr {
  getter(Expr_t, expr);

public:
  ExprStmt(Expr_t expr) : _expr(std::move(expr)) {}

  void accept(IExprVisitor &visitor) override { visitor.visit(*this); }
};

class VoidStmt : public Expr {
public:
  VoidStmt() {}
  void accept(IExprVisitor &visitor) override { visitor.visit(*this); }
};

class ASTree {
private:
  std::vector<Expr_t> stmts;

public:
  ASTree();

  void add_expression(Expr_t expr) { stmts.push_back(std::move(expr)); }

  auto to_string() -> std::string;
};

} // namespace AST

#endif // !AST_HPP
