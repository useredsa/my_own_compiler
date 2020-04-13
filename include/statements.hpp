#ifndef STATEMENTS_HPP
#define STATEMENTS_HPP

#include <iostream>
#include <string>
#include <vector>
#include "statement.hpp"
#include "expressions.hpp"
#include "identifiers.hpp"

namespace AST {

class t_if_then_statement : public t_statement {
  private:
    t_expression* cond_;
    t_statement* conseq_;
  
  public:
    t_if_then_statement(t_expression *cond, t_statement *conseq)
     : cond_(cond), conseq_(conseq) {  }

    void llvm_put(std::ostream& os, int& local_var_count) {
        //TODO
    }

    void print(int lvl) {
      std::string tabs = std::string(lvl, '\t');
      std::cout << tabs << "if\n";
      cond_->print(lvl+1);
      std::cout << tabs << "then\n";
      conseq_->print(lvl+1);
    }
};

class t_if_then_else_statement : public t_if_then_statement {
  private:
    t_statement *alternative;
  
  public:
    t_if_then_else_statement(t_expression *cond, t_statement *conseq,
                             t_statement *alt)
     : t_if_then_statement(cond, conseq), alternative(alt) {  }

    void llvm_put(std::ostream& os, int& local_var_count) {
        //TODO
    }

    void print(int lvl) {
      std::string tabs = std::string(lvl, '\t');
      t_if_then_statement::print(lvl);
      std::cout << tabs << "else\n";
      alternative->print(lvl+2);
    }
};

class t_while_statement : public t_statement {
  private:
    t_expression *condition;
    t_statement *statement;
  
  public:
    t_while_statement(t_expression *cond, t_statement *statement)
     : condition(cond), statement(statement) {  }

    void llvm_put(std::ostream& os, int& local_var_count) {
        //TODO
    }

    void print(int lvl) {
      std::string tabs = std::string(lvl, '\t');
      std::cout << tabs << "while\n";
      condition->print(lvl+1);
      std::cout << tabs << "\tdo\n";
      statement->print(lvl+1);
    }
};

class t_for_statement : public t_statement {
  private:
    t_id* control_id;
    t_expression *begin, *end;
    t_statement *statement;
  
  public:
    t_for_statement(t_id* ctrl_id, t_expression *begin, t_expression *end,
                    t_statement *statement)
     : control_id(ctrl_id), begin(begin), end(end), statement(statement) {  }

    void llvm_put(std::ostream& os, int& local_var_count) {
        //TODO
    }

    void print(int lvl) {
      std::string tabs = std::string(lvl, '\t');
      std::cout << tabs << "for\n";
      control_id->print(lvl+1);
      std::cout << tabs << "\t:=\n";
      begin->print(lvl+1);
      std::cout << tabs << "\tto\n";
      end->print(lvl+1);
      std::cout << tabs << "\tdo\n";
      statement->print(lvl+1);
    }
};

class t_assignment : public t_statement {
  private:
    t_id* id;
    t_expression *exp;

  public:
    t_assignment(t_id* id, t_expression *exp) : id(id), exp(exp) {  }

    void llvm_put(std::ostream& os, int& local_var_count);

    void print(int lvl);
};

class t_read : public t_statement {
  private:
    std::vector<t_id*>* ids;
    
  public:
    t_read(std::vector<t_id*>* ids) : ids(ids) {  }

    void llvm_put(std::ostream& os, int& local_var_count);

    void print(int lvl);
};

class t_write : public t_statement {
  private:
    t_expressions* exps;
    
  public:
    t_write(t_expressions* exps) : exps(exps) {  }

    void llvm_put(std::ostream& os, int& local_var_count);

    void print(int lvl);

};

//TODO only for testing
static t_assignment auxiliar(nullptr, nullptr);

}

#endif // STATEMENTS_HPP

