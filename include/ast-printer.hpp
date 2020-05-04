#ifndef AST_PRINTER_HPP
#define AST_PRINTER_HPP

#include <iomanip>
#include "ast.hpp"
#include "identifiers.hpp"
#include "statements.hpp"
#include "log.hpp"

namespace compiler {

class AstPrinter {
  public:
    AstPrinter(std::ostream& os) : log(os) {  }

    void operator()(ast::Stmt stmt) {
        std::visit(*this, stmt);
    }

    void operator()(ast::Exp exp) {
        std::visit(*this, exp);
    }

    void operator()(const ast::Prog* prog) {
        log << "Prog\n";
        log << "funDecls:\n";
        PrintIndented(prog->funs);
        log << "\n";
    }

    void operator()(const ast::RVar rvar) {
        log << "RVar " << rvar.id->name() << "\n";
    }
    void operator()(const ast::Var* var) {
        log << "Var " << var->id()->name() << " = \n";
        PrintIndented(var->val());
    }

    void operator()(const ast::Fun* func) {
        log << "Fun " << func->id()->name() << "\n";
        log << "type: " << func->rtype().id->name() << "\n";
        log << "arguments:\n";
        PrintIndented(func->args());
        log << "declarations\n";
        PrintIndented(func->var_dcls());
        log << "Statements:\n";
        PrintIndented(func->stmts());
        log << "\n";
    }

    void operator()(const ast::EmptyStmt* empty) {
        log << "EmptyStmt\n";
    }

    void operator()(const ast::CompStmt* stmts) {
        for (auto stmt : *stmts) {
            (*this)(stmt);
        }
    }

    void operator()(const ast::Assig* assig) {
        log << "Assig\n";
        PrintIndented(assig->rvar);
        log << "=\n";
        PrintIndented(assig->exp);
    }

    void operator()(const ast::IfStmt* if_stmt) {
        log << "IfStmt\n";
        PrintIndented(if_stmt->exp);
        log << "Then\n";
        PrintIndented(if_stmt->stmt);
        log << "Else\n";
        PrintIndented(if_stmt->alt_stmt);
    }

    void operator()(const ast::WhileStmt* while_stmt) {
        log << "WhileStmt\n";
        PrintIndented(while_stmt->exp);
        log << "Do\n";
        PrintIndented(while_stmt->stmt);
    }

    void operator()(const ast::ForStmt* for_stmt) {
        log << "ForStmt\n";
        PrintIndented(for_stmt->rvar);
        log << " :=\n";
        PrintIndented(for_stmt->start_exp);
        log << "To\n";
        PrintIndented(for_stmt->end_exp);
        log << "Do\n";
        PrintIndented(for_stmt->stmt);
    }

    void operator()(const ast::ReadStmt* read_stmt) {
        log << "ReadStmt\n";
        PrintIndented(read_stmt->rvars);
    }

    void operator()(const ast::WriteStmt* write_stmt) {
        log << "WriteStmt\n";
        PrintIndented(write_stmt->exps);
    }

    void operator()(const ast::NoExp* no_exp) {
        log << "NoExp\n";
    }

    void operator()(const ast::IntLit* int_lit) {
        log << "IntLit " << int_lit->lit << "\n";
    }

    void operator()(const ast::StrLit* str_lit) {
        log << "StrLit " << std::quoted(*str_lit->lit) << "\n";
    }

    void operator()(const ast::FunCall* call) {
        log << "FunCall to " << call->rfun.id->name() << "\n";
        PrintIndented(call->args);
    }

    void operator()(const identifiers::Id* id) {
        log << "Id " << id->name() << "\n";
    }

    template<ast::UnaryOperators op>
    void operator()(const ast::UnaOp<op>* una_op) {
        log << "UnaOp<" << static_cast<char>(op) << ">\n";
        PrintIndented(una_op->exp);
    }

    template<ast::BinaryOperators op>
    void operator()(const ast::BinOp<op>* bin_op) {
        log << "BinOp<" << static_cast<char>(op) << ">\n";
        PrintIndented(bin_op->lhs);
        log << static_cast<char>(op) << "\n";
        PrintIndented(bin_op->rhs);
    }


  private:
    class TreeLogger : public Logger {
      public:
        TreeLogger(std::ostream& os) : Logger(os), prefix_() {  }

        inline void AddPrefix(const std::string& s) {
            prefix_ += s;
        }

        inline void PopPrefix(const std::string& s) {
            assert(s.size() <= prefix_.size());
            // assert(s == prefix_.substr(prefix_.size()-s.size(), s.size()));
            prefix_.resize(prefix_.size()-s.size());
        }

      protected:
        inline virtual std::string prefix() { return prefix_; }

      private:
        std::string prefix_;
    } log;

    template<typename T>
    inline void PrintIndented(T obj) {
        log.AddPrefix("    ");
        (*this)(obj);
        log.PopPrefix("    ");
    }

    template<typename T>
    inline void PrintIndented(std::vector<T> vec) {
        log.AddPrefix("    ");
        for (auto obj : vec) {
            (*this)(obj);
        }
        log.PopPrefix("    ");
    }
};

} // namespace compiler

#endif // AST_PRINTER_HPP

