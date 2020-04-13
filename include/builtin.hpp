#ifndef BUILTIN_HPP
#define BUILTIN_HPP

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

    t_type* exp_type();

    std::string llvm_eval(std::ostream& os, int& local_var_count);

    void print(int lvl);

  private:
    const int lit;
};

/**
 * @brief A string literal
 */
class t_str_lit : public t_expression {
  public:
    t_str_lit(std::string *lit);

    t_type* exp_type();

    std::string llvm_eval(std::ostream& os, int& local_var_count);

    void print(int lvl);

  private:
    std::string *lit;
    std::string llvm_id = "TODO";
};

/**
 * @brief A function call
 */
class t_function_call : public t_expression {
  public:
    t_function_call(t_name* object, t_expressions* args) : object(object), args(args) {  }

    t_type* exp_type();

    std::string llvm_eval(std::ostream& os, int& local_var_count);

    void print(int lvl);

  private:
    t_name* object;
    t_expressions* args;
};

class t_unary_op : public t_expression {
  public:
    enum valid_op {
        minus
    };  //TODO Esto no sé dónde va

    t_unary_op(const valid_op op, t_expression *exp);

    t_type* exp_type();

    //TODO No puedo poner virtual en el .cpp
    virtual std::string llvm_eval(std::ostream& os, int& local_var_count);

    void print(int lvl);
  
  private:
    const valid_op op;
    t_expression *exp;
};

class t_binary_op : public t_expression {
    //TODO estoy suponiendo el tipo de ambos operandos es int.
    // Hay que comprobar tipos y buscar el operador apropiado.
  public:
    enum valid_op {
        plus, minus, asterisk, slash
    };

    static std::string op_string(valid_op op);
    
    t_binary_op(const valid_op op, t_expression *lhs, t_expression *rhs);

    t_type* exp_type();

    std::string llvm_eval(std::ostream& os, int& local_var_count);

    void print(int lvl);

  private:
    const valid_op op;
    t_expression *l, *r;
};

}

#endif // BUILTIN_HPP

