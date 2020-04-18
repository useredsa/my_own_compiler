#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include <string>
#include <vector>

#include "type.hpp"

namespace AST {

class t_id;

/**
 * @brief An expression
 * 
 * A class that represent an evaluable expression whose
 * value can be printed
 */
class t_expression /*TODO remove : public t_print_item*/ {
  public:
    /**
     * @brief Type of the expression
     */
    virtual t_id *exp_type() = 0;  //TODO This should return t_type?
    //TODO remove? virtual int reg() = 0;
    virtual void print(int lvl) = 0;
    /**
     * @brief String value of the expression
     * 
     * Emits the neccesary code to compute the value of the expression
     * and returns it
     */
    virtual std::string llvm_eval(std::ostream& os, int& local_var_count) = 0;
};

/**
 * @brief A list of expressions
 * 
 * //TODO Why deserves to be a class?
 */
class t_expressions : public std::vector<t_expression*> {
  public:
    t_expressions();

    t_expressions(t_expression *first);

    void print(int lvl);
};

}  // namespace AST

#endif // EXPRESSION_HPP
