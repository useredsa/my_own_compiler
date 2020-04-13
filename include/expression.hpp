#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include <iostream>
#include <string>
#include <vector>
#include "type.hpp"

namespace AST {

class t_id;

class t_expression /*TODO remove : public t_print_item*/ {
  public:
    virtual t_id* exp_type() = 0;
    // virtual int reg() = 0;
    virtual void print(int lvl) = 0;
    virtual std::string llvm_eval(std::ostream& os, int& local_var_count) = 0;
};

class t_expressions : public std::vector<t_expression*> {
  public:
    t_expressions() {  }

    t_expressions(t_expression *first) {
        this->push_back(first);
    }

    void print(int lvl) {
        std::cout << std::string(lvl, '\t') << "expressions:\n";
        for (auto exp : *this) {
            exp->print(lvl+1);
        }
    }
};

}

#endif // EXPRESSION_HPP

