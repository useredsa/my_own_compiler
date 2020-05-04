#ifndef EXPRESSIONS_HPP
#define EXPRESSIONS_HPP

#include <assert.h>
#include <vector>
#include <string>
#include <variant>
#include "ast_defs.hpp"

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
 * @brief An integer literal
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
 * @brief A string literal
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

    /**
     * @brief For builtin specific literals, it's possible to specify
     * the id used in the code translation
     */
    StrLit(std::string* lit, std::string llvm_id)  : lit(lit), llvm_id(llvm_id) {
        assert(lit != nullptr);
        program_str_lits.push_back(this);
    }

    //TODO add destructor that erases from the list!
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
    Exp exp;

    explicit UnaOp(Exp exp) : exp(exp) {  };
};

/**
 * @brief A binary operation
 */
template<BinaryOperators op>
struct BinOp {
    Exp lhs;
    Exp rhs;

    //TODO estoy suponiendo el tipo de ambos operandos es int.
    // Hay que comprobar tipos y buscar el operador apropiado.
    BinOp(Exp lhs, Exp rhs) : lhs(lhs), rhs(rhs) {  };
};

} // namespace ast

} // namespace compiler

#endif // EXPRESSIONS_HPP

