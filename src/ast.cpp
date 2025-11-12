#include "ast.hpp"
#include "lexer.hpp"
#include <format>
#include <iomanip>
#include <iostream>
#include <ostream>
#include <sstream>

void AST::ExprPrinter::visit(Expr &expr) { ss << "Typical expr" << std::endl; }

void AST::ExprPrinter::visit(ValueExpr &expr) {
  ss << std::format("Value({}; {})", tok2str(expr.value()),
                    expr.value().getValue());
}

void AST::ExprPrinter::visit(AST::FunCallExpr &expr) {
  ss << "FunCall(" << expr.ident().getValue() << "; ";
  for (size_t i = 0; i < expr.args().size(); i++) {
    if (i > 0)
      ss << ", ";
    expr.args()[i]->accept(*this);
  }
  ss << ")";
}

void AST::ExprPrinter::visit(GroupingExpr &expr) {
  ss << "Grouping(";
  expr.expr()->accept(*this);
  ss << ")";
}

void AST::ExprPrinter::visit(UnaryExpr &expr) {
  ss << std::format("Unary({}; ", tok2str(expr.op()));
  expr.right()->accept(*this);
  ss << ")";
}

void AST::ExprPrinter::visit(BinaryExpr &expr) {
  ss << "Binary(" << std::endl;
  ss << std::setw(4) << " ";
  expr.left()->accept(*this);
  ss << ";" << std::endl << std::setw(4) << " ";
  ss << tok2str(expr.op());
  ss << ";" << std::endl << std::setw(4) << " ";
  expr.right()->accept(*this);
  ss << std::endl << ")";
}

void AST::ExprPrinter::visit(TernaryExpr &expr) {
  ss << "Ternary(" << std::endl;

  ss << std::setw(4) << " ";
  expr.cond()->accept(*this);

  ss << ";" << std::endl << std::setw(4) << " ";
  expr.expr_true()->accept(*this);

  ss << ";" << std::endl << std::setw(4) << " ";
  expr.expr_false()->accept(*this);

  ss << std::endl << ")";
}

void AST::ExprPrinter::visit(AssignExpr &expr) {
  ss << "Assign(" << expr.ident().getValue() << "; ";
  expr.right()->accept(*this);
  ss << ")" << std::endl;
}

void AST::ExprPrinter::visit(DefineExpr &expr) {
  ss << "Define(" << expr.ident().getValue() << "; " << expr.type().getValue();
  if (expr.expr().has_value()) {
    ss << std::endl << std::setw(4) << " ";
    expr.expr().value()->accept(*this);
  } else {
    ss << "; {}";
  }

  ss << ")";
}

void AST::ExprPrinter::visit(ImportStmt &stmt) {
  ss << "Import(" << stmt.litteral().getValue() << ")" << std::endl;
}

void AST::ExprPrinter::visit(AST::ArgDefineExpr &expr) {
  ss << "Arg(" << expr.ident().getValue() << "; " << tok2str(expr.type())
     << ")";
}

void AST::ExprPrinter::visit(AST::BlockStmt &stmt) {
  ss << "Block(";

  for (size_t i = 0; i < stmt.stmts().size(); i++) {
    if (i > 0) {
      ss << ", ";
    }
    stmt.stmts()[i]->accept(*this);
  }

  ss << ")" << std::endl;
}

void AST::ExprPrinter::visit(AST::FnDefineStmt &stmt) {
  ss << "FnDef(" << stmt.ident().getValue() << "; ";

  for (size_t i = 0; i < stmt.args().size(); i++) {
    if (i > 0) {
      ss << ", ";
    }
    stmt.args()[i].accept(*this);
  }

  ss << "; ";

  stmt.block().accept(*this);

  ss << ")" << std::endl;
}

void AST::ExprPrinter::visit(AST::ReturnStmt &stmt) {
  ss << "Return(";
  stmt.expr()->accept(*this);
  ss << ")" << std::endl;
}

void AST::ExprPrinter::visit(AST::ExprStmt &stmt) {
  ss << "ExprStmt(";
  stmt.expr()->accept(*this);
  ss << ")" << std::endl;
}

void AST::ExprPrinter::visit(AST::IfStmt &stmt) {
  ss << "If(";
  stmt.cond()->accept(*this);

  ss << "; ";
  stmt.true_block().accept(*this);

  if (stmt.elif_block().has_value()) {
    ss << "; elif(";
    stmt.elif_block().value()->accept(*this);
    ss << ");";
  }

  if (stmt.else_block().has_value()) {
    ss << "; else(";
    stmt.else_block().value().accept(*this);
    ss << ")";
  }

  ss << ")" << std::endl;
}

void AST::ExprPrinter::visit(AST::WhileStmt &stmt) {
  ss << "While(";
  stmt.cond()->accept(*this);
  ss << "; ";
  stmt.block().accept(*this);
  ss << ")" << std::endl;
}

void AST::ExprPrinter::visit(AST::ForStmt &stmt) {
  ss << "For(";
  stmt.init()->accept(*this);
  ss << "; ";
  stmt.cond()->accept(*this);
  ss << "; ";
  stmt.step();
  ss << "; ";
  stmt.block().accept(*this);
  ss << ")" << std::endl;
}

void AST::ExprPrinter::visit(AST::InterfaceStmt &stmt) {
  ss << "Interface(" << stmt.ident().getValue() << "; ";
  stmt.block().accept(*this);
  ss << ")" << std::endl;
}

void AST::ExprPrinter::visit(AST::ClassStmt &stmt) {
  ss << "Class(" << stmt.ident().getValue() << "; " << stmt.super()->getValue()
     << "; ";

  for (size_t i = 0; i < stmt.interfaces().value().size(); i++) {
    if (i > 0) {
      ss << ", ";
    }

    ss << stmt.interfaces().value()[i].getValue();
  }

  ss << "; ";
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
