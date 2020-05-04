#ifndef AST_DEFS_HPP
#define AST_DEFS_HPP

#include <variant>

namespace compiler {

namespace identifiers {

class Id;

} // namespace identifiers

namespace ast {

struct Var;
class Type;
class Fun;

union RVar {
    identifiers::Id* id;
    Var* var;

    RVar() {  }
    explicit RVar(identifiers::Id* id) : id(id) {  }
};

union RType {
    identifiers::Id* id;
    Type* ty;

    RType() {  }
    explicit RType(identifiers::Id* id) : id(id) {  }
};

union RFun {
    identifiers::Id* id;
    Fun* fun;

    RFun() {  }
    explicit RFun(identifiers::Id* id) : id(id) {  }
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

template<UnaryOperators op>  struct UnaOp;
template<BinaryOperators op> struct BinOp;
class Id;
struct IntLit;
struct StrLit;
struct FunCall;
struct NoExp;


using Exp = std::variant<
    UnaOp<kUnaMinus>*,
    BinOp<kPlus>*,
    BinOp<kBinMinus>*,
    BinOp<kAsterisk>*,
    BinOp<kSlash>*,
    RVar,
    IntLit*,
    StrLit*,
    FunCall*,
    NoExp*
>;

struct Assig;
struct IfStmt;
struct WhileStmt;
struct ForStmt;
struct WriteStmt;
struct ReadStmt;
struct CompStmt;
struct EmptyStmt;

using Stmt = std::variant<
    Assig*,
    IfStmt*,
    WhileStmt*,
    ForStmt*,
    WriteStmt*,
    ReadStmt*,
    CompStmt*,
    EmptyStmt*
>;

} // namespace ast

} // namespace compiler

#endif // AST_DEFS_HPP

