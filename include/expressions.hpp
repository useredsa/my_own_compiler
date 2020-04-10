#ifndef EXPRESSIONS_HPP
#define EXPRESSIONS_HPP

#include <iostream>
#include <string>
#include <vector>

#include "io.hpp"

namespace AST {

class t_expression /*TODO remove : public t_print_item*/ {
  public:
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

class t_unary_op : public t_expression {
  public:
    enum valid_op {
        minus
    };

    const valid_op op;
    t_expression *exp;

    t_unary_op(const valid_op op, t_expression *exp)
      : op(op), exp(exp) {  }

    virtual std::string llvm_eval(std::ostream& os, int& local_var_count) {
        //TODO
        return "TODO";
    }

    void print(int lvl) {
        std::string tabs(lvl, '\t');
        std::cout << tabs << "t_unary_op:\n";
        std::cout << tabs << "\t-\n";
        exp->print(lvl+1);
        std::cout << '\n';
    }
};

class t_binary_op : public t_expression {
  public:
    enum valid_op {
        plus, minus, asterisk, slash
    };
    
    const valid_op op;
    t_expression *l, *r;
    
    t_binary_op(const valid_op op, t_expression *lhs, t_expression *rhs)
      : op(op), l(lhs), r(rhs) {  }

    virtual std::string llvm_eval(std::ostream& os, int& local_var_count) {
        //TODO estoy suponiendo el tipo de ambos operandos es int. Hay que comprobar
        // tipos y buscar el operador apropiado.
        std::string lhs = l->llvm_eval(os, local_var_count);
        std::string rhs = r->llvm_eval(os, local_var_count);
        //TODO hay que separar el tipo y el resultado :S
        rhs.erase(0, 4);
        std::string result = "%" + std::to_string(local_var_count++);
        os << "\t" << result << " = add nsw " << lhs << ", " << rhs << "\n";
        return "i32 " + result;
    }

    void print(int lvl) {
        std::string tabs(lvl, '\t');
        std::cout << tabs << "t_binary_op:\n";
        l->print(lvl+1);
        std::cout << tabs << '\t';
        switch (op) {
          case plus:
            std::cout << '+';
            break;
          case minus:
            std::cout << '-';
            break;
          case asterisk:
            std::cout << '*';
            break;
          case slash:
            std::cout << '/';
            break;
        }
        std::cout << '\n';
        r->print(lvl+1);
        std::cout << '\n';
    }
};

}
#endif // EXPRESSIONS_HPP

