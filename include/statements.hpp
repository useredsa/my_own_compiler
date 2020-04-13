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
    t_expression *cond;
    t_statement *conseq;
  
  public:
    t_if_then_statement(t_expression *cond, t_statement *conseq)
     : cond(cond), conseq(conseq) {  }

    void llvm_put(std::ostream& os, int& local_var_count) {
        //TODO
    }

    void print(int lvl) {
      std::string tabs = std::string(lvl, '\t');
      std::cout << tabs << "if\n";
      cond->print(lvl+1);
      std::cout << tabs << "then\n";
      conseq->print(lvl+1);
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

    void llvm_put(std::ostream& os, int& local_var_count) {
        // std::string value = exp->llvm_eval(os, local_var_count);
        // std::string addr = id->llvm_ref(os, local_var_count);
        // os << "\tstore " << exp->exp_type()->llvm_name() << value << ", "
        //    << id->exp_type()->llvm_name() << "* " << addr << ", align 4\n";
        //TODO
    }

    void print(int lvl) {
        std::cout << std::string(lvl, '\t') << "assignment\n";
        id->print(lvl+1);
        exp->print(lvl+1);
    }
};

class t_write : public t_statement {
  private:
    t_expressions* exps;
    
  public:
    t_write(t_expressions* exps) : exps(exps) {  }

    void llvm_put(std::ostream& os, int& local_var_count) {
        for (auto exp : *exps) {
            //TODO
            // std::string ref = exp->llvm_eval(os, local_var_count);
            // os << "\t%" << local_var_count++ << " = call i32 (i8*, ...) @printf(i8* "
            //    << "getelementptr inbounds ([3 x i8], [3 x i8]* @.io.int, i32 0, i32 0), "
            //    << exp->exp_type()->llvm_name() << " " << ref << ")\n";
        }
    }

    void print(int lvl) {
        std::cout << std::string(lvl, '\t') << "write statement\n";
        for (auto exp : *exps) {
            exp->print(lvl+1);
        }
    }
};

class t_read : public t_statement {
  private:
    std::vector<t_id*>* ids;
    
  public:
    t_read(std::vector<t_id*>* ids) : ids(ids) {  }

    void llvm_put(std::ostream& os, int& local_var_count) {
        for (auto id : *ids) {
            // std::string ref = id->llvm_ref(os, local_var_count);
            // os << "\t%" << local_var_count++ << " = call i32 (i8*, ...) @__isoc99_scanf(i8* "
            //    << "getelementptr inbounds ([3 x i8], [3 x i8]* @.io.int, i32 0, i32 0), "
            //    << id->exp_type()->llvm_name() << "* " << ref << ")\n";
        }
    }

    void print(int lvl) {
        std::cout << std::string(lvl, '\t') << "read statement\n";
        for (auto id : *ids) {
            id->print(lvl+1);
        }
    }
};

//TODO only for testing
static t_assignment auxiliar(t_id::named("auxiliarTODO"), t_id::named("auxiliarTODO"));

}

#endif // STATEMENTS_HPP

