#ifndef STATEMENTS_HPP
#define STATEMENTS_HPP

#include <iostream>
#include <string>
#include <vector>
#include "statement.hpp"
#include "expressions.hpp"
#include "identifiers.hpp"

namespace compiler {

namespace ast {

struct Assig  : public IStmt {
    Id* id;
    IExp* exp;

    Assig (Id* id, IExp* exp) : id(id), exp(exp) {  }

    void llvm_put(std::ostream& os, int& local_var_count);

    void print(int lvl);
};

struct IfStmt : public IStmt {
    IExp* exp;
    IStmt* stmt;
    IStmt* alt_stmt;

    IfStmt(IExp* exp, IStmt* stmt, IStmt* alt_stmt = nullptr)
            : exp(exp), stmt(stmt), alt_stmt(alt_stmt) {  }

    void llvm_put(std::ostream& os, int& local_var_count);

    void print(int lvl);
};

struct WhileStmt : public IStmt {
    IExp* exp;
    IStmt* stmt;
    WhileStmt(IExp *exp, IStmt* stmt)
            : exp(exp), stmt(stmt) {  }

    void llvm_put(std::ostream& os, int& local_var_count);

    void print(int lvl);
};

struct ForStmt : public IStmt {
    Id* id;  //TODO Implement scoping?
    IExp* start_exp;
    IExp* end_exp;
    IStmt* stmt;

    ForStmt(Id* id, IExp* start_exp, IExp* end_exp, IStmt* stmt);

    void llvm_put(std::ostream& os, int& local_var_count);

    void print(int lvl);
};

struct WriteStmt : public IStmt {
    Exps* exps;

    WriteStmt(Exps* exps) : exps(exps) {  }

    void llvm_put(std::ostream& os, int& local_var_count);

    void print(int lvl);
};

struct ReadStmt : public IStmt {
    std::vector<Id*>* ids;

    ReadStmt(std::vector<Id*>* ids) : ids(ids) {  }

    void llvm_put(std::ostream& os, int& local_var_count);

    void print(int lvl);
};

} // namespace ast

} // namespace compiler

#endif // STATEMENTS_HPP

