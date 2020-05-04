#include "ast.hpp"

#include <iostream>
#include "builtin.hpp"
#include "types.hpp"
#include "expressions.hpp"

using std::vector;
using std::string;

namespace compiler {

namespace ast {

Var::Var(identifiers::Id* id, RType rtype, Exp val)
     : id_(id), rtype_(rtype), val_(val) {
    id_->RegisterAsVariable(this);
}

Var::Var(identifiers::Id* id, RType rtype)
     : Var(id, rtype, new ast::NoExp) {

}

// void Dcls::AddConstants(const std::vector<std::pair<Id*, IntLit*>>&  cons) {
//     for (auto [id, val] : cons) {
//         if (!id->RegisterAsConstant(val)) {
//             //TODO semantic_log << "ERROR..."
//         }
//         constants.push_back(id);
//     }
// }

// void Dcls::AddIdentifiers(const std::vector<Id*>& ids, Id* type) {
//     for (Id* id : ids) {
//         if (!id->RegisterAsVariable(type)) {
//             semantic_log << "ERROR: identifier " <<  id->name() << " already in use. Unavailable to define as a variable\n";
//         }
//         variables.push_back(id);
//     }
// }

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
    std::cerr << "registering " << id->name() << std::endl;
    if (!id->RegisterFunction(this)) {
        //TODO semantic_log << "ERROR: the function cannot be named " << name << " a variable or a function with the same signature is registered with that name\n";
    }
}

Prog::Prog(string&& name,
           vector<Fun*>&& funs,
           vector<Var*>&& dcls,
           vector<Stmt>&& stmts)
    : name(name),
      funs(funs) {
    this->funs.push_back(new Fun(identifiers::NewId("main"),
                                 RType(builtin::IntTypeId()),
                                 {},
                                 std::move(dcls),
                                 std::move(stmts)));
}

} //namespace ast

} // namespace compiler

