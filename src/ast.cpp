#include "ast.hpp"

#include <iostream>
#include "builtin.hpp"
#include "types.hpp"
#include "expressions.hpp"

using std::vector;
using std::string;

namespace compiler {

namespace ast {

Type::Type(identifiers::Id* id)
        : id_(id) {
    id->RegisterAsType(this);
}

vector<Var*> program_vars;

Var::Var(identifiers::Id* id, RType rtype, const Exp& val, bool is_constant)
        : id_(id), rtype_(rtype), val_(val), is_constant_(is_constant) {
    id_->RegisterAsVariable(this);
    program_vars.push_back(this);
}

Var::Var(identifiers::Id* id, RType rtype)
        : Var(id, rtype, new ast::NoExp()) {  }

Fun::Fun(identifiers::Id* id,
         RType rtype,
         vector<Var*>&& args,
         vector<Var*>&& var_dcls,
         vector<Stmt>&& stmts)
        : id_(id),
          rtype_(rtype),
          args_(args),
          var_dcls_(var_dcls),
          stmts_(stmts) {
    id->RegisterFunction(this);
    //IMPROVEMENT Errors are being print inside that function, but could be print here
    // saying what couldn't be registered instead.
}

Prog::Prog(string&& name,
           vector<Fun*>&& funs,
           vector<Var*>&& dcls,
           vector<Stmt>&& stmts)
        : name(name),
          funs(funs) {
    // The id main is already registered to prevent other functions from being called
    // main. Therefore, the call should be GetId instead of NewId
    this->funs.push_back(new Fun(identifiers::GetId("main"),
                                 RType(builtin::IntTypeId()),
                                 {},
                                 std::move(dcls),
                                 std::move(stmts)));
}

} //namespace ast

} // namespace compiler

