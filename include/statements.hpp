#ifndef STATEMENTS_HPP
#define STATEMENTS_HPP

#include <iostream>
#include <string>
#include <vector>
#include "statement.hpp"
#include "expressions.hpp"
#include "identifiers.hpp"

namespace AST {

class t_assignment : public t_statement {
  public:
    t_assignment(t_id* id, t_expression *exp) : id_(id), exp_(exp) {  }

    void llvm_put(std::ostream& os, int& local_var_count);

    void print(int lvl);

  private:
    t_id* id_;
    t_expression *exp_;
};

class t_if_then_statement : public t_statement {
  public:
    t_if_then_statement(t_expression *cond, t_statement *conseq)
            : cond_(cond), conseq_(conseq) {  }

    void llvm_put(std::ostream& os, int& local_var_count);

    void print(int lvl);

  private:
    t_expression* cond_;
    t_statement* conseq_;
};

class t_if_then_else_statement : public t_if_then_statement {
  public:
    t_if_then_else_statement(t_expression *cond, t_statement *conseq,
                             t_statement *alt)
            : t_if_then_statement(cond, conseq), alternative_(alt) {  }

    void llvm_put(std::ostream& os, int& local_var_count);

    void print(int lvl);
  
  private:
    t_statement *alternative_;
};

class t_while_statement : public t_statement {
  public:
    t_while_statement(t_expression *cond, t_statement *statement)
            : cond_(cond), statement_(statement) {  }

    void llvm_put(std::ostream& os, int& local_var_count);

    void print(int lvl);
  
  private:
    t_expression *cond_;
    t_statement *statement_;
};

class t_for_statement : public t_statement {
  public:
    t_for_statement(t_id* ctrl_id, t_expression *begin, t_expression *end,
                    t_statement *statement);

    void llvm_put(std::ostream& os, int& local_var_count);

    void print(int lvl);
  
  private:
    t_id* control_id_;  //TODO Implement scoping?
    t_expression *begin_, *end_;
    t_statement *statement_;
};

class t_write : public t_statement {
  public:
    t_write(t_expressions* exps) : exps(exps) {  }

    void llvm_put(std::ostream& os, int& local_var_count);

    void print(int lvl);
    
  private:
    t_expressions* exps;
};

class t_read : public t_statement {
  public:
    t_read(std::vector<t_id*>* ids) : ids(ids) {  }

    void llvm_put(std::ostream& os, int& local_var_count);

    void print(int lvl);
    
  private:
    std::vector<t_id*>* ids;
};

}  // namespace AST

#endif // STATEMENTS_HPP

