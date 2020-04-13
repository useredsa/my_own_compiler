#ifndef AST_HPP
#define AST_HPP

#include "functions.hpp"
#include "statements.hpp"
#include "identifiers.hpp"

namespace AST {

/**
 * @brief Root node of the AST
 * 
 * //TODO
 */
class T_program {
  public:
    T_program();

    T_program(t_functions *funcs, t_declarations *decls, t_statements *stmts);

    /**
     * @brief Prints the entire AST
     */
    void print(int lvl = 0);

    /**
     * @brief Generates the LLVM code of the program represented by the AST
     */
    void llvm_output(std::ostream& os, int local_var_count = 1);

  private:
    t_functions *functions_;
    t_declarations *declarations_;
    t_statements *statements_;

};

}  // namespace

#endif // AST_HPP

