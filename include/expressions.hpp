#ifndef EXPRESSIONS_HPP
#define EXPRESSIONS_HPP

#include "expressions.hpp"
#include "identifiers.hpp"
#include "types.hpp"
#include <vector>
#include <string>

namespace compiler {

namespace ast {

/**
 * @brief An integer literal
 */
struct IntLit : public IExp {
    int lit;

    IntLit(int lit) : lit(lit) {  };

    Id* exp_type();

    std::string llvm_eval(std::ostream& os, int& local_var_count);

    void print(int lvl);
};

/**
 * @brief A string literal
 */
struct StrLit : public IExp {
    std::string* lit;
    std::string llvm_id;

    StrLit(std::string* lit);

    StrLit(std::string* lit, const std::string& llvm_id);

    //TODO add destructor that erases from the list!

    Id* exp_type();

    std::string llvm_eval(std::ostream& os, int& local_var_count);

    void print(int lvl);
};

/** String literals of the program */
extern std::vector<StrLit*> program_str_lits;

/**
 * @brief A function call
 */
struct FuncCall : public IExp {
    Id* id;
    Exps* args;

    FuncCall(Id* id, Exps* args) : id(id), args(args) {  };

    Id *exp_type();

    std::string llvm_eval(std::ostream& os, int& local_var_count);

    void print(int lvl);
};

enum UnaryOperators : char {
    kUnaMinus = '-',
};

enum BinaryOperators : char {
    kPlus     = '+',
    kBinMinus = '-',
    kAsterisk = '*',
    kSlash    = '/',
};

/**
 * @brief An unary operation
 */
struct UnaOp : public IExp {
    UnaryOperators op;
    IExp *exp;

    UnaOp(UnaryOperators op, IExp* exp) : op(op), exp(exp) {  };

    Id* exp_type();

    virtual std::string llvm_eval(std::ostream& os, int& local_var_count);

    void print(int lvl);
};

/**
 * @brief A binary operation
 */
struct BinOp : public IExp {
    const BinaryOperators op;
    IExp* lhs;
    IExp* rhs;

    //TODO estoy suponiendo el tipo de ambos operandos es int.
    // Hay que comprobar tipos y buscar el operador apropiado.
    BinOp(const BinaryOperators op, IExp* lhs, IExp* rhs) : op(op), lhs(lhs), rhs(rhs) {  };

    Id *exp_type();

    std::string llvm_eval(std::ostream& os, int& local_var_count);

    void print(int lvl);
};

} // namespace ast

} // namespace compiler

#endif // EXPRESSIONS_HPP

