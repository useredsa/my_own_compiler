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
class t_program {
  public:
    t_program();

    t_program(t_functions *funcs, t_declarations *decls, t_statements *stmts);

    /**
     * @brief Prints the entire AST
     */
    void print(int lvl = 0);

    /**
     * @brief Generates the LLVM code of the program represented by the AST
     */
    void llvm_output(std::ostream& os, int local_var_count = 1);

    t_functions* functions_;
  private:
    t_function main_;

};

}  // namespace

#endif // AST_HPP

