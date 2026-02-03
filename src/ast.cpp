#include "ast.hpp"
#include "lexer.hpp"
#include <format>
#include <iomanip>
#include <iostream>
#include <ostream>
#include <sstream>
#include <stdexcept>

void AST::ExprPrinter::tab() { indent_level++; }

void AST::ExprPrinter::indent() {
  for (size_t i = 0; i < indent_level; i++) {
    ss << "  ";
  }
}

void AST::ExprPrinter::untab(const std::source_location &location) {
  if (indent_level == 0) {
    throw std::runtime_error(
        std::format("Trying to untab 0 indent at: {}", location.line()));
  }
  indent_level--;
}

void AST::ExprPrinter::newline() {
  ss << std::endl;
  indent();
}

void AST::ExprPrinter::visit(Expr &expr) { ss << "Typical expr" << std::endl; }

void AST::ExprPrinter::visit(ValueExpr &expr) {
  ss << std::format("Value({}; {})", tok2str(expr.value()),
                    expr.value().getValue());
}

void AST::ExprPrinter::visit(AST::FunCallExpr &expr) {
  ss << "FunCall(" << expr.ident().getValue() << "(";
  for (size_t i = 0; i < expr.args().size(); i++) {
    if (i > 0)
      ss << ", ";
    expr.args()[i]->accept(*this);
  }
  ss << "))";
}

void AST::ExprPrinter::visit(GroupingExpr &expr) {
  ss << "Grouping(";
  expr.expr()->accept(*this);
  ss << ")";
}

void AST::ExprPrinter::visit(UnaryExpr &expr) {
  ss << std::format("Unary({} ", tok2str(expr.op()));
  expr.right()->accept(*this);
  ss << ")";
}

void AST::ExprPrinter::visit(BinaryExpr &expr) {
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

void AST::ExprPrinter::visit(TernaryExpr &expr) {
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

void AST::ExprPrinter::visit(AssignExpr &expr) {
  ss << "Assign(";
  tab();
  newline();
  ss << expr.ident().getValue() << " = ";
  expr.right()->accept(*this);
  newline();
  ss << ")" << std::endl;
  untab();
  indent();
}

void AST::ExprPrinter::visit(DefineExpr &expr) {
  ss << "Define(" << expr.ident().getValue() << ": " << tok2str(expr.type());
  if (expr.expr().has_value()) {
    ss << " = ";
    expr.expr().value()->accept(*this);
  } else {
    ss << " = nil";
  }

  ss << ")";
}

void AST::ExprPrinter::visit(ImportStmt &stmt) {
  ss << "Import(" << stmt.litteral().getValue() << ")" << std::endl;
}

void AST::ExprPrinter::visit(AST::ArgDefineExpr &expr) {
  ss << "Arg(" << expr.ident().getValue() << ": " << tok2str(expr.type())
     << ")";
}

void AST::ExprPrinter::visit(AST::BlockStmt &stmt) {
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

void AST::ExprPrinter::visit(AST::FnDefineStmt &stmt) {
  ss << "FnDef(" << stmt.ident().getValue() << "(";

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

void AST::ExprPrinter::visit(AST::ReturnStmt &stmt) {
  ss << "Return(";
  stmt.expr()->accept(*this);
  ss << ")";
}

void AST::ExprPrinter::visit(AST::ExprStmt &stmt) {
  ss << "ExprStmt(";
  stmt.expr()->accept(*this);
  ss << ")";
}

void AST::ExprPrinter::visit(AST::IfStmt &stmt) {
  ss << "If(";
  stmt.cond()->accept(*this);

  ss << ") : ";
  stmt.true_block().accept(*this);

  if (stmt.elif_block().has_value()) {
    ss << " elif ";
    stmt.elif_block().value()->accept(*this);
  }

  if (stmt.else_block().has_value()) {
    ss << " else ";
    stmt.else_block().value().accept(*this);
  }
}

void AST::ExprPrinter::visit(AST::WhileStmt &stmt) {
  ss << "While(";
  stmt.cond()->accept(*this);
  ss << ") " << std::endl;
  stmt.block().accept(*this);
}

void AST::ExprPrinter::visit(AST::ForStmt &stmt) {
  ss << "For(";
  stmt.init()->accept(*this);
  ss << "; ";
  stmt.cond()->accept(*this);
  ss << "; ";
  stmt.step();
  ss << ")";
  stmt.block().accept(*this);
}

void AST::ExprPrinter::visit(AST::InterfaceFnDefineStmt &stmt) {
  ss << "InterfaceFn(" << stmt.ident().getValue() << "(";

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

void AST::ExprPrinter::visit(AST::InterfaceStmt &stmt) {
  ss << "Interface(" << stmt.ident().getValue() << " ";
  stmt.block().accept(*this);
  ss << ")";
  newline();
  newline();
}

void AST::ExprPrinter::visit(AST::ClassStmt &stmt) {
  ss << "Class(" << stmt.ident().getValue();

  if (stmt.super().has_value()) {
    ss << " : " << stmt.super().value().getValue();
  }

  if (stmt.interfaces().has_value() && stmt.interfaces().value().size() > 0) {
    ss << " < ";
    for (size_t i = 0; i < stmt.interfaces().value().size(); i++) {
      if (i > 0) {
        ss << ", ";
      }

      ss << stmt.interfaces().value()[i].getValue();
    }
  }

  ss << " ";
  stmt.block().accept(*this);
  ss << ")";
  newline();
  newline();
}

void AST::ExprPrinter::visit(AST::ClassField &stmt) {
  ss << "ClassField(";

  if (stmt.is_private()) {
    ss << "private ";
  } else {
    ss << "public ";
  }

  ss << stmt.ident().getValue() << ": " << tok2str(stmt.type()) << ")";
}

void AST::ExprPrinter::visit(AST::ClassFnDefineStmt &stmt) {
  ss << "ClassFnDefine( ";

  if (stmt.is_private()) {
    ss << "private ";
  } else {
    ss << "public ";
  }

  ss << stmt.ident().getValue() << "(";
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

void AST::ExprPrinter::visit(AST::VoidStmt &stmt) {
  ss << "VoidStmt" << std::endl;
}

auto AST::ExprPrinter::get_string() -> std::string { return ss.str(); }

AST::ASTree::ASTree() {}

auto AST::ASTree::to_string() -> std::string {
  ExprPrinter printer;

  for (auto &&stmt : stmts) {
    stmt->accept(printer);
  }

  return printer.get_string();
}
