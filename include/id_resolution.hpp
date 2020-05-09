#ifndef ID_RESOLUTION_HPP
#define ID_RESOLUTION_HPP

#include <vector>

#include "ast_defs.hpp"
#include "ast.hpp"

namespace compiler {

namespace identifiers {


class NameResolution {
  public:
    static void Do();
    static void FirstPass();
    static void SecondPass();
    
    void operator()(ast::Prog* prog);
    void operator()(ast::Fun* func);
    void operator()(ast::Var* var);
    
    void operator()(ast::Stmt& stmt);
    void operator()(ast::EmptyStmt* empty);
    void operator()(ast::CompStmt* stmts);
    void operator()(ast::Assig* assig);
    void operator()(ast::IfStmt* if_stmt);
    void operator()(ast::WhileStmt* while_stmt);
    void operator()(ast::ForStmt* for_stmt);
    void operator()(ast::ReadStmt* read_stmt);
    void operator()(ast::WriteStmt* write_stmt);
    
    
    ast::Type* GetType(ast::Exp& exp);
    ast::Type* GetType(ast::RVar& rvar);
    ast::Type* operator()(ast::NoExp* no_exp);
    ast::Type* operator()(ast::IntLit* int_lit);
    ast::Type* operator()(ast::StrLit* str_lit);
    ast::Type* operator()(ast::FunCall* call);
    ast::Type* operator()(ast::RVar& rvar);
    template<ast::UnaryOperators op>
    ast::Type* operator()(ast::UnaOp<op>* una_op);
    template<ast::BinaryOperators op>
    ast::Type* operator()(ast::BinOp<op>* bin_op);

  private:
    static ast::Fun* FromFunSig(Id*, const std::vector<ast::Type*>& signature);

    template<typename T>
    void operator()(std::vector<T> vec);
};

} // namespace identifiers

} // namespace compiler

#endif // ID_RESOLUTION_HPP

