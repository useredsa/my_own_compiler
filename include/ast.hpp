#ifndef AST_HPP
#define AST_HPP

#include "function.hpp"
#include "statements.hpp"
#include "expressions.hpp"
#include "identifiers.hpp"

namespace compiler {

namespace ast {

/**
 * @brief Root node of the AST
 * 
 * //TODO
 */
struct Program {
    std::vector<Function*> funcs;
    Function main;

    Program(std::vector<Function*>&& funcs, Dcls* decls, std::vector<Stmt>&& stmts);

    // /**
    //  * @brief Generates the LLVM code of the program represented by the AST
    //  */
    // void llvm_output(std::ostream& os, int local_var_count = 1);
};

}  // namespace ast

} // namespace compiler

#endif // AST_HPP

