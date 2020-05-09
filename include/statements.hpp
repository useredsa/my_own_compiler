#ifndef STATEMENTS_HPP
#define STATEMENTS_HPP

#include <string>
#include <vector>
#include <variant>
#include "expressions.hpp"
#include "identifiers.hpp"

namespace compiler {

namespace ast {

struct EmptyStmt {  };

struct CompStmt : public std::vector<Stmt> {
    explicit CompStmt(std::vector<Stmt>&& stmts) : std::vector<Stmt>(stmts) {  }
};

struct Assig {
    RVar rvar;
    Exp exp;

    Assig (RVar rvar, Exp exp) : rvar(rvar), exp(exp) {  }
};

struct IfStmt {
    Exp exp;
    Stmt stmt;
    Stmt alt_stmt;

    IfStmt(Exp exp, Stmt stmt)
            : exp(exp), stmt(stmt), alt_stmt(new EmptyStmt) {  }

    IfStmt(Exp exp, Stmt stmt, Stmt alt_stmt)
            : exp(exp), stmt(stmt), alt_stmt(alt_stmt) {  }
};

struct WhileStmt {
    Exp exp;
    Stmt stmt;

    WhileStmt(Exp exp, Stmt stmt)
            : exp(exp), stmt(stmt) {  }
};

struct ForStmt {
    RVar rvar;
    Exp start_exp;
    Exp end_exp;
    Stmt stmt;

   ForStmt(RVar rvar, Exp start_exp, Exp end_exp, Stmt stmt)
        : rvar(rvar), start_exp(start_exp), end_exp(end_exp), stmt(stmt) {  }

};

struct WriteStmt {
    std::vector<Exp> exps;

    explicit WriteStmt(std::vector<Exp>&& exps) : exps(exps) {  }
};

struct ReadStmt {
    std::vector<RVar> rvars;

    explicit ReadStmt(std::vector<RVar>&& rvars) : rvars(rvars) {  }
};

} // namespace ast

} // namespace compiler

#endif // STATEMENTS_HPP

