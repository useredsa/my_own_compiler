#ifndef AST_HPP
#define AST_HPP

#include "function.hpp"
#include "statements.hpp"
#include "identifiers.hpp"

namespace compiler {

namespace ast {

/**
 * @brief Root node of the AST
 * 
 * //TODO
 */
class Program {
  public:
    Program(Functions* funcs, Dcls* decls, Stmts* stmts);

    /**
     * @brief Prints the entire AST
     */
    void print(int lvl = 0);

    /**
     * @brief Generates the LLVM code of the program represented by the AST
     */
    void llvm_output(std::ostream& os, int local_var_count = 1);

  private:
    Functions* funcs_;
    Function main_;
};

}  // namespace ast

} // namespace compiler

#endif // AST_HPP

