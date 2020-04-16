#ifndef EXPRESSIONS_HPP
#define EXPRESSIONS_HPP

#include "expressions.hpp"
#include "identifiers.hpp"
#include "types.hpp"

namespace AST {

/**
 * @brief An integer literal
 */
class t_int_lit : public t_expression {
  public:
    t_int_lit(int lit);

    t_id *exp_type();

    std::string llvm_eval(std::ostream& os, int& local_var_count);

    void print(int lvl);

  private:
    const int lit_;
};

/**
 * @brief A string literal
 */
class t_str_lit : public t_expression {
  public:
    t_str_lit(std::string *lit);

    t_id *exp_type();

    std::string llvm_eval(std::ostream& os, int& local_var_count);

    void print(int lvl);

  private:
    std::string *lit_;
    std::string llvm_id_ = "TODO";
};

/**
 * @brief A function call
 */
class t_function_call : public t_expression {
  public:
    t_function_call(t_id *name, t_expressions *args);

    t_id *exp_type();

    std::string llvm_eval(std::ostream& os, int& local_var_count);

    void print(int lvl);

  private:
    t_id *name_;
    t_expressions *args_;
};

/**
 * @brief An unary operation
 */
class t_unary_op : public t_expression {
  public:
    enum valid_op {
        minus
    };

    t_unary_op(const valid_op op, t_expression *exp);

    t_id *exp_type();

    virtual std::string llvm_eval(std::ostream& os, int& local_var_count);

    void print(int lvl);
  
  private:
    const valid_op op_;
    t_expression *exp_;
};

/**
 * @brief A binary operation
 */
class t_binary_op : public t_expression {
    //TODO estoy suponiendo el tipo de ambos operandos es int.
    // Hay que comprobar tipos y buscar el operador apropiado.
  public:
    enum valid_op {
        plus, minus, asterisk, slash
    };

    static std::string op_string(valid_op op);
    
    t_binary_op(const valid_op op, t_expression *lhs, t_expression *rhs);

    t_id *exp_type();

    std::string llvm_eval(std::ostream& os, int& local_var_count);

    void print(int lvl);

  private:
    const valid_op op_;
    t_expression *l_, *r_;
};

}
#endif // EXPRESSIONS_HPP

