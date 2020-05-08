#ifndef LLVM_HPP
#define LLVM_HPP

#include <ostream>
#include <iomanip>
#include "ast_defs.hpp"
#include "ast.hpp"
#include "expressions.hpp"
#include "builtin.hpp"

namespace compiler {

namespace llvm {

/**
 * @brief A reference to a value in llvm.
 * 
 * Use cases: Result of an expression.
 * 
 * Examples:
 * {type -> IntType, val = 12}
 * {type -> IntType, val = "%var"}
 * {type -> StrType, val = "%7"}
 */
struct ComputedExp {
    ast::Type* type;
    std::string val;
};

/**
 * @brief Generates the LLVM code of the program represented by the AST
 */
class Translator {
  public:
    explicit Translator(std::ostream& os) : os(os) {  }

    /**
     * @brief Outputs an llvm file out of an ast.
     */
    void Output(ast::Prog* prog);

    void Output(ast::Fun* fun);
    // void AllocaConstants(Dcls* dcls);

    void Output(ast::Stmt stmt);

    void operator()(ast::EmptyStmt* empty_stmt);
    void operator()(ast::CompStmt* cmp_stmt);
    void operator()(ast::Assig* assig);
    void operator()(ast::IfStmt* if_stmt);
    void operator()(ast::WhileStmt* while_stmt);
    void operator()(ast::ForStmt* for_stmt);
    void operator()(ast::ReadStmt* read_stmt);
    void operator()(ast::WriteStmt* write_stmt);

    ComputedExp Eval(ast::Exp exp);

    ComputedExp operator()(ast::NoExp* no_exp);
    template<ast::UnaryOperators op>
    ComputedExp operator()(ast::UnaOp<op>* una_op);
    template<ast::BinaryOperators op>
    ComputedExp operator()(ast::BinOp<op>* bin_op);
    ComputedExp operator()(ast::RVar rvar);
    ComputedExp operator()(ast::IntLit* int_lit);
    ComputedExp operator()(ast::StrLit* str_lit);
    ComputedExp operator()(ast::FunCall* fun_call);

    /**
     * @brief Outputs as global variables the program's str literals
     */
    void LlvmPutStrLits() {
        for (ast::StrLit* str_lit : ast::program_str_lits) {
            os << str_lit->llvm_id << " = private unnamed_addr constant ["
               << str_lit->lit->size()+1 << " x i8] c\"" << *str_lit->lit
               << "\\00\", align 1\n";
        }
    }

  private:
    std::ostream& os;
    int local_var_count;

    void EasyAlloca(ast::Var* var);
    void EasyStore(const std::string& val, ast::Var* var);
};

} // namespace llvm

} // namespace compiler

#endif // LLVM_HPP

