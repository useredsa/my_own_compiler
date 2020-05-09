#ifndef EXPRESSIONS_HPP
#define EXPRESSIONS_HPP

#include <assert.h>
#include <vector>
#include <string>
#include <variant>
#include "ast_defs.hpp"
#include "identifiers.hpp"

namespace compiler {

namespace ast {

/**
 * @brief Can be used where an expression is optional.
 * 
 * For instance, in the declaration of variables.
 */
struct NoExp {
    
};

/**
 * @brief An int literal
 */
struct IntLit {
    int lit;

    /**
     * The default value is provided because bison needs a default
     * constructor.
     */
    explicit IntLit(int lit = 0) : lit(lit) {  };
};

/** String literals of the program */
extern std::vector<StrLit*> program_str_lits;

/**
 * @brief A str literal
 */
struct StrLit {
    std::string* lit;
    std::string llvm_id;

    /**
     * @brief This constructor is used for parsing.
     * 
     * A default value is provided because bison needs one,
     * but you shouldn't instantiate the struct with a nullptr.
     */
    explicit StrLit(std::string* lit = nullptr)
        : StrLit(lit, "@.strlit." + std::to_string(program_str_lits.size())) {  }
    //IMPROVEMENT There should be a a destructor that erased from the list
    // in case we wanted to optimize and modify the ast

    /**
     * @brief For builtin specific literals, it's possible to specify
     * the id used in the code translation
     */
    StrLit(std::string* lit, std::string llvm_id)  : lit(lit), llvm_id(llvm_id) {
        assert(lit != nullptr);
        program_str_lits.push_back(this);
    }
};

/**
 * @brief A function call
 */
struct FunCall {
    RFun rfun;
    std::vector<Exp> args;

    FunCall(RFun rfun, std::vector<Exp>&& args) : rfun(rfun), args(args) {  };
};

/**
 * @brief An unary operation
 */
template<UnaryOperators op>
struct UnaOp {
    RFun rfun;
    Exp exp;

    explicit UnaOp(const Exp& exp)
        : rfun(identifiers::GetId(std::string(".operator") + static_cast<char>(op))),
          exp(exp) {  };
};

/**
 * @brief A binary operation
 */
template<BinaryOperators op>
struct BinOp {
    RFun rfun;
    Exp lhs;
    Exp rhs;

    BinOp(const Exp& lhs, const Exp& rhs)
        : rfun(identifiers::GetId(std::string(".operator") + static_cast<char>(op))),
          lhs(lhs),
          rhs(rhs) {  };
};

} // namespace ast

} // namespace compiler

#endif // EXPRESSIONS_HPP

