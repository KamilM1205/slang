/**
 * @file ast.cpp
 * @brief Реализует функционал вывода AST дерева в консоль
 * @details Хранит имплементацию методов класса ExprPrinter для вывода структуры
 * AST дерева.
 * @author Kamil Meftahutdinov
 * @date 2026
 */
#include "ast.hpp"
#include "lexer.hpp"
#include <format>
#include <iomanip>
#include <iostream>
#include <ostream>
#include <sstream>
#include <stdexcept>

void SLang::AST::ExprPrinter::tab() { indent_level++; }

void SLang::AST::ExprPrinter::indent() {
  for (size_t i = 0; i < indent_level; i++) {
    ss << "  ";
  }
}

void SLang::AST::ExprPrinter::untab(const std::source_location &location) {
  if (indent_level == 0) {
    throw std::runtime_error(
        std::format("Trying to untab 0 indent at: {}", location.line()));
  }
  indent_level--;
}

void SLang::AST::ExprPrinter::newline() {
  ss << std::endl;
  indent();
}

void SLang::AST::ExprPrinter::visit(Expr &expr) {
  ss << "Typical expr" << std::endl;
}

void SLang::AST::ExprPrinter::visit(ValueExpr &expr) {
  ss << std::format("Value({}; {})", tok2str(expr.value()),
                    expr.value().get_value());
}

void SLang::AST::ExprPrinter::visit(SLang::AST::FunCallExpr &expr) {
  ss << "FunCall(" << expr.ident().get_value() << "(";
  for (size_t i = 0; i < expr.args().size(); i++) {
    if (i > 0)
      ss << ", ";
    expr.args()[i]->accept(*this);
  }
  ss << "))";
}

void SLang::AST::ExprPrinter::visit(GroupingExpr &expr) {
  ss << "Grouping(";
  expr.expr()->accept(*this);
  ss << ")";
}

void SLang::AST::ExprPrinter::visit(UnaryExpr &expr) {
  ss << std::format("Unary({} ", tok2str(expr.op()));
  expr.right()->accept(*this);
  ss << ")";
}

void SLang::AST::ExprPrinter::visit(BinaryExpr &expr) {
  ss << "Binary( op: " << tok2str(expr.op());
  tab();
  newline();
  ss << "left: ";
  expr.left()->accept(*this);
  newline();
  ss << "right: ";
  expr.right()->accept(*this);
  newline();
  ss << ")";
  untab();
}

void SLang::AST::ExprPrinter::visit(TernaryExpr &expr) {
  ss << "Ternary(" << std::endl;

  ss << std::setw(4) << " ";
  expr.cond()->accept(*this);

  ss << ";" << std::endl << std::setw(4) << " ";
  expr.expr_true()->accept(*this);

  ss << ";" << std::endl << std::setw(4) << " ";
  expr.expr_false()->accept(*this);

  newline();
  ss << ")";
}

void SLang::AST::ExprPrinter::visit(AssignExpr &expr) {
  ss << "Assign(";
  tab();
  newline();
  ss << expr.ident().get_value() << " = ";
  expr.right()->accept(*this);
  newline();
  ss << ")" << std::endl;
  untab();
  indent();
}

void SLang::AST::ExprPrinter::visit(DefineExpr &expr) {
  ss << "Define(" << expr.ident().get_value() << ": " << tok2str(expr.type());
  if (expr.expr().has_value()) {
    ss << " = ";
    expr.expr().value()->accept(*this);
  } else {
    ss << " = nil";
  }

  ss << ")";
}

void SLang::AST::ExprPrinter::visit(ImportStmt &stmt) {
  ss << "Import(" << stmt.litteral().get_value() << ")" << std::endl;
}

void SLang::AST::ExprPrinter::visit(SLang::AST::ArgDefineExpr &expr) {
  ss << "Arg(" << expr.ident().get_value() << ": " << tok2str(expr.type())
     << ")";
}

void SLang::AST::ExprPrinter::visit(SLang::AST::BlockStmt &stmt) {
  ss << "Block {";
  tab();

  for (size_t i = 0; i < stmt.stmts().size(); i++) {
    newline();
    stmt.stmts()[i]->accept(*this);
  }
  untab();

  newline();
  ss << "}";
}

void SLang::AST::ExprPrinter::visit(SLang::AST::FnDefineStmt &stmt) {
  ss << "FnDef(" << stmt.ident().get_value() << "(";

  for (size_t i = 0; i < stmt.args().size(); i++) {
    if (i > 0) {
      ss << ", ";
    }
    stmt.args()[i].accept(*this);
  }

  ss << ") -> ";

  if (stmt.ret_type().has_value()) {
    ss << tok2str(stmt.ret_type().value());
  } else {
    ss << "nil";
  }

  ss << "; ";

  stmt.block().accept(*this);

  ss << ")";
  newline();
  newline();
}

void SLang::AST::ExprPrinter::visit(SLang::AST::ReturnStmt &stmt) {
  ss << "Return(";
  stmt.expr()->accept(*this);
  ss << ")";
}

void SLang::AST::ExprPrinter::visit(SLang::AST::ExprStmt &stmt) {
  ss << "ExprStmt(";
  stmt.expr()->accept(*this);
  ss << ")";
  newline();
}

void SLang::AST::ExprPrinter::visit(SLang::AST::IfStmt &stmt) {
  ss << "If(";
  stmt.cond()->accept(*this);

  ss << ") { ";
  tab();
  newline();
  stmt.true_block().accept(*this);
  untab();
  ss << "} ";

  for (auto &stmt : stmt.elif_block()) {
    ss << " elif (";
    stmt->cond()->accept(*this);
    ss << ") {";
    tab();
    newline();
    stmt->true_block().accept(*this);
    untab();
    newline();
    ss << "}";
  }

  if (stmt.else_block().has_value()) {
    ss << " else ";
    stmt.else_block().value().accept(*this);
  }
}

void SLang::AST::ExprPrinter::visit(SLang::AST::WhileStmt &stmt) {
  ss << "While(";
  stmt.cond()->accept(*this);
  ss << ") " << std::endl;
  stmt.block().accept(*this);
}

void SLang::AST::ExprPrinter::visit(SLang::AST::ForStmt &stmt) {
  ss << "For(";
  stmt.init()->accept(*this);
  ss << "; ";
  stmt.cond()->accept(*this);
  ss << "; ";
  stmt.step();
  ss << ")";
  stmt.block().accept(*this);
}

void SLang::AST::ExprPrinter::visit(SLang::AST::InterfaceFnDefineStmt &stmt) {
  ss << "InterfaceFn(" << stmt.ident().get_value() << "(";

  for (size_t i = 0; i < stmt.args().size(); i++) {
    if (i > 0) {
      ss << ", ";
    }
    stmt.args()[i].accept(*this);
  }

  ss << "))";

  if (stmt.ret_type().has_value()) {
    ss << " -> " << tok2str(stmt.ret_type().value());
  }
}

void SLang::AST::ExprPrinter::visit(SLang::AST::InterfaceStmt &stmt) {
  ss << "Interface(" << stmt.ident().get_value() << " ";
  stmt.block().accept(*this);
  ss << ")";
  newline();
  newline();
}

void SLang::AST::ExprPrinter::visit(SLang::AST::ClassStmt &stmt) {
  ss << "Class(" << stmt.ident().get_value();

  if (stmt.super().has_value()) {
    ss << " : " << stmt.super().value().get_value();
  }

  if (stmt.interfaces().has_value() && stmt.interfaces().value().size() > 0) {
    ss << " < ";
    for (size_t i = 0; i < stmt.interfaces().value().size(); i++) {
      if (i > 0) {
        ss << ", ";
      }

      ss << stmt.interfaces().value()[i].get_value();
    }
  }

  ss << " ";
  stmt.block().accept(*this);
  ss << ")";
  newline();
  newline();
}

void SLang::AST::ExprPrinter::visit(SLang::AST::ClassField &stmt) {
  ss << "ClassField(";

  if (stmt.is_private()) {
    ss << "private ";
  } else {
    ss << "public ";
  }

  ss << stmt.ident().get_value() << ": " << tok2str(stmt.type()) << ")";
}

void SLang::AST::ExprPrinter::visit(SLang::AST::ClassFnDefineStmt &stmt) {
  ss << "ClassFnDefine( ";

  if (stmt.is_private()) {
    ss << "private ";
  } else {
    ss << "public ";
  }

  ss << stmt.ident().get_value() << "(";
  for (size_t i = 0; i < stmt.args().size(); i++) {
    if (i > 0) {
      ss << ", ";
    }

    stmt.args()[i].accept(*this);
  }

  ss << ") -> ";
  if (stmt.ret_type().has_value()) {
    ss << tok2str(stmt.ret_type().value()) << " ";
  } else {
    ss << "nil ";
  }
  stmt.block().accept(*this);
}

void SLang::AST::ExprPrinter::visit(SLang::AST::ClassMemberGetter &stmt) {
  ss << "ClassMemberGetter(";
  tab();
  newline();
  ss << "left: ";
  stmt.left()->accept(*this);
  newline();
  ss << "right: ";
  stmt.right()->accept(*this);
  untab();
  ss << ")";
  newline();
}

void SLang::AST::ExprPrinter::visit(SLang::AST::ClassMemberSetter &stmt) {
  ss << "ClassMemberSetter(";
  tab();
  newline();
  ss << "left: ";
  stmt.left()->accept(*this);
  newline();
  ss << "right: ";
  stmt.right()->accept(*this);
  untab();
  ss << ")";
  newline();
}

void SLang::AST::ExprPrinter::visit(SLang::AST::ClassMemberCall &stmt) {
  ss << "ClassMemberCall(";
  tab();
  newline();
  ss << "method: ";
  stmt.method()->accept(*this);
  newline();
  ss << "right: ";
  stmt.right()->accept(*this);
  untab();
  ss << ")";
  newline();
}

void SLang::AST::ExprPrinter::visit(SLang::AST::ClassMemberAccess &stmt) {
  ss << "ClassMemberAccess(";
  tab();
  newline();
  ss << "left expr: ";
  stmt.left()->accept(*this);
  newline();
  ss << " right expr: ";
  stmt.right()->accept(*this);
  untab();
  ss << ")";
  newline();
}

void SLang::AST::ExprPrinter::visit(SLang::AST::VoidStmt &stmt) {
  ss << "VoidStmt" << std::endl;
}

auto SLang::AST::ExprPrinter::get_string() -> std::string { return ss.str(); }

SLang::AST::ASTree::ASTree() {}

auto SLang::AST::ASTree::to_string() -> std::string {
  ExprPrinter printer;

  for (auto &&stmt : stmts) {
    stmt->accept(printer);
  }

  return printer.get_string();
}

auto SLang::AST::ASTree::get_stmts() -> std::vector<Expr_t> * { return &stmts; }
