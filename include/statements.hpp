#ifndef STATEMENTS_HPP
#define STATEMENTS_HPP

#include <string>
#include <vector>
#include <variant>
#include "expressions.hpp"

namespace compiler {

namespace ast {

class Id;
struct Assig;
struct IfStmt;
struct WhileStmt;
struct ForStmt;
struct WriteStmt;
struct ReadStmt;
struct CompStmt;
struct EmptyStmt;

using Stmt = std::variant<
    Assig*,
    IfStmt*,
    WhileStmt*,
    ForStmt*,
    WriteStmt*,
    ReadStmt*,
    CompStmt*,
    EmptyStmt*
>;

struct EmptyStmt {  };

struct CompStmt : public std::vector<Stmt> {
    CompStmt(std::vector<Stmt>&& stmts) : std::vector<Stmt>(stmts) {  }
};

struct Assig {
    Id* id;
    Exp exp;

    Assig (Id* id, Exp exp) : id(id), exp(exp) {  }
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
    Id* id;  //TODO Implement scoping?
    Exp start_exp;
    Exp end_exp;
    Stmt stmt;

    ForStmt(Id* id, Exp start_exp, Exp end_exp, Stmt stmt);
};

struct WriteStmt {
    std::vector<Exp> exps;

    WriteStmt(std::vector<Exp>&& exps) : exps(exps) {  }
};

struct ReadStmt {
    std::vector<Id*> ids;

    ReadStmt(std::vector<Id*>&& ids) : ids(ids) {  }
};

} // namespace ast

} // namespace compiler

#endif // STATEMENTS_HPP

