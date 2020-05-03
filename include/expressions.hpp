#ifndef EXPRESSIONS_HPP
#define EXPRESSIONS_HPP

#include <vector>
#include <string>
#include <variant>

namespace compiler {

namespace ast {

enum UnaryOperators : char {
    kUnaMinus = '-',
};

enum BinaryOperators : char {
    kPlus     = '+',
    kBinMinus = '-',
    kAsterisk = '*',
    kSlash    = '/',
};

template<UnaryOperators op>  struct UnaOp;
template<BinaryOperators op> struct BinOp;
class Id;
struct IntLit;
struct StrLit;
struct FuncCall;


using Exp = std::variant<
    UnaOp<kUnaMinus>*,
    BinOp<kPlus>*,
    BinOp<kBinMinus>*,
    BinOp<kAsterisk>*,
    BinOp<kSlash>*,
    Id*,
    IntLit*,
    StrLit*,
    FuncCall*
>;

/**
 * @brief An integer literal
 */
struct IntLit {
    int lit;

    IntLit(int lit = 0) : lit(lit) {  };
};

/** String literals of the program */
extern std::vector<StrLit*> program_str_lits;

/**
 * @brief A string literal
 */
struct StrLit {
    std::string* lit;
    std::string llvm_id;

    StrLit(std::string* lit = nullptr); //TODO added for bison

    StrLit(std::string* lit, const std::string& llvm_id);

    //TODO add destructor that erases from the list!
};

/**
 * @brief A function call
 */
struct FuncCall {
    Id* id;
    std::vector<Exp> args;

    FuncCall(Id* id, std::vector<Exp>&& args) : id(id), args(args) {  };
};

/**
 * @brief An unary operation
 */
template<UnaryOperators op>
struct UnaOp {
    Exp exp;

    UnaOp(Exp exp) : exp(exp) {  };
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

