#include "function.hpp"

#include "identifiers.hpp"
#include "log.hpp"

namespace compiler {

namespace ast {

void Dcls::AddConstants(const std::vector<std::pair<Id*, IntLit*>>&  cons) {
    for (auto [id, val] : cons) {
        if (!id->RegisterAsConstant(val)) {
            //TODO semantic_log << "ERROR..."
        }
        constants.push_back(id);
    }
}

void Dcls::AddIdentifiers(const std::vector<Id*>& ids, Id* type) {
    for (Id* id : ids) {
        if (!id->RegisterAsVariable(type)) {
            semantic_log << "ERROR: identifier " <<  id->name() << " already in use. Unavailable to define as a variable\n";
        }
        variables.push_back(id);
    }
}

// void Dcls::llvm_put_constants(std::ostream& os) {
    
// }

// void Dcls::llvm_put_variables(std::ostream& os) {
//     for (Id* id : variables) {
//         if (not id->IsAVariable()) {
//             semantic_log << "ERROR: not a variable!\n"; //TODO
//             continue;
//         }
//         id->llvm_var_alloca(os);
//     }
// }

Function::Function(Id* type,
                   Id* name,
                   const std::vector<std::pair<Id*, Id*>>& args,
                   Dcls* dcls,
                   std::vector<Stmt>&& statements)
    : name_(name->name()),
      type_(type),
      dcls_(dcls),
      args_(),
      signature_(),
      statements_(statements) {
    for (auto [type, id] : args) {
        id->RegisterAsVariable(type);
        signature_.push_back(type);
        args_.push_back(id);
    }
    // Important: Register after construction is finished
    if (!name->RegisterFunction(this)) {
        //TODO semantic_log << "ERROR: the function cannot be named " << name << " a variable or a function with the same signature is registered with that name\n";
    }
}

// void Function::llvm_put(std::ostream& os) {
//     dcls_->llvm_put_constants(os);
//     os << "; Function\n";
//     os << "define i32 @" << name_ << "(";
//     for (int i = 0; i < (int) signature_.size(); ++i) {
//         if (i)
//             os << ", ";
//         os << signature_[i]->llvm_type_name();
//     }
//     os << ") {\n";
//     for (int i = 0; i < (int) signature_.size(); ++i) {
//         os << "\t%" << args_[i]->name() << " = alloca " << signature_[i]->llvm_type_name()
//            << ", align 4\n";
//     }
//     for (int i = 0; i < (int) signature_.size(); ++i) {
//         os << "\tstore " << signature_[i]->llvm_type_name() << " %" << i << ", "
//            << signature_[i]->llvm_type_name() << "* %" << args_[i]->name() << ", align 4\n";
//     }
//     int local_var_count = 1+signature_.size();
//     dcls_->llvm_put_variables(os);
//     statements_->llvm_put(os, local_var_count);
//     os << "\tret i32 0\n"; //TODO return type!
//     os << "}\n\n";
// }

//TODO considerar usar using std::string porque es horrible escribir estos nombres
// std::string Function::llvm_put_call(std::ostream& os,
//                                       int& local_var_count,
//                                       std::vector<std::string*> params) {
//     return "TODO";
// }

} // namespace ast

} // namespace compiler
