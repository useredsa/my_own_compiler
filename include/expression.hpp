#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include <string>
#include <vector>

#include "type.hpp"

namespace compiler {
    
namespace ast {

class Id;

/**
 * @brief An expression
 * 
 * A class that represent an evaluable expression whose
 * value can be printed
 */
class IExp {
  public:
    /**
     * @brief Type of the expression
     */
    virtual Id* exp_type() = 0;  //TODO This should return t_type?

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
class Exps : public std::vector<IExp*> {
  public:
    Exps() {  };

    Exps(IExp* first);

    void print(int lvl);
};

} // namespace ast

} // namespace compiler

#endif // EXPRESSION_HPP
