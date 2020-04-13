#ifndef BUILTIN_HPP
#define BUILTIN_HPP

#include "expressions.hpp"
#include "identifiers.hpp"
#include "types.hpp"

namespace AST {

class t_int_lit : public t_expression {
  public:
    const int lit;

    t_int_lit(int lit) : lit(lit) {  }

    t_type* exp_type() {
        return new builtin::t_int();
    }

    std::string llvm_eval(std::ostream& os, int& local_var_count) {
        return std::to_string(lit);
    }

    void print(int lvl) {
        std::cout << std::string(lvl, '\t') << "int_lit: " << lit << '\n';
    }
};

class t_str_lit : public t_expression {
  private:
    std::string *lit;
    std::string llvm_id = "TODO";
    
  public:
    t_str_lit(std::string *lit) : lit(lit) {  }

    t_type* exp_type() {
        return new builtin::t_str();
    }

    std::string llvm_eval(std::ostream& os, int& local_var_count) {
        return "getelementptr inbounds ([4 x i8], [4 x i8]* " + llvm_id
               + ", i32 0, i32 0)";
    }

    void print(int lvl) {
        std::cout << std::string(lvl, '\t') << "str_lit: " << *lit << '\n';
    }
};

class t_function_call : public t_expression {
  public:
    t_name* object;
    t_expressions* args;

    t_function_call(t_name* object, t_expressions* args) : object(object), args(args) {  }

    t_type* exp_type() {
        //TODO
        return new builtin::t_int();
    }

    std::string llvm_eval(std::ostream& os, int& local_var_count) {
        //TODO
        return "TODO";
    }

    void print(int lvl) {
        std::cout << std::string(lvl, '\t') << "call to " << "TODO\n";
        for (auto arg : *args) {
            args->print(lvl+1);
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

    t_type* exp_type() {
        return new builtin::t_int();
    }

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
    //TODO estoy suponiendo el tipo de ambos operandos es int.
    // Hay que comprobar tipos y buscar el operador apropiado.
  public:
    enum valid_op {
        plus, minus, asterisk, slash
    };

    static std::string op_string(valid_op op) {
        switch (op) {
          case plus:
            return "+";
            break;
          case minus:
            return "-";
            break;
          case asterisk:
            return "*";
            break;
          case slash:
            return "/";
            break;
          default:
            return "UNRECOGNIZED OP";
        }
    }
    
    const valid_op op;
    t_expression *l, *r;
    
    t_binary_op(const valid_op op, t_expression *lhs, t_expression *rhs) :
        op(op), l(lhs), r(rhs) {  }

    t_type* exp_type() {
        return new builtin::t_int();
    }

    std::string llvm_eval(std::ostream& os, int& local_var_count) {
        std::string lhs = l->llvm_eval(os, local_var_count);
        std::string rhs = r->llvm_eval(os, local_var_count);
        std::string result = "%" + std::to_string(local_var_count++);
        os << "\t" << result << " = add nsw " << l->exp_type()->llvm_name()
           << " " << lhs << ", " << rhs << "\n";
        return result;
    }

    void print(int lvl) {
        std::string tabs(lvl, '\t');
        std::cout << tabs << "t_binary_op:\n";
        l->print(lvl+1);
        std::cout << tabs << '\t' << op_string(op) << '\n';
        r->print(lvl+1);
        std::cout << '\n';
    }
};

}

#endif // BUILTIN_HPP

