#include "expressions.hpp"

#include "errors.hpp"
#include "namespace.hpp"
#include "function.hpp"

using std::vector;
using std::string;

namespace compiler {

namespace ast {

Id* IntLit::exp_type() {
    return builtin::IntTypeId();
}

string IntLit::llvm_eval(std::ostream& os, int& local_var_count) {
    return std::to_string(lit);
}

void IntLit::print(int lvl) {
    std::cout << string(lvl, '\t') << "int_lit: " << lit << '\n';
}

vector<StrLit*> program_str_lits;

StrLit::StrLit(string* lit, const string& llvm_id) : lit(lit), llvm_id(llvm_id) {
    program_str_lits.push_back(this);
}

StrLit::StrLit(string* lit)
    : StrLit(lit, "@.strlit." + std::to_string(program_str_lits.size())) {  }

Id* StrLit::exp_type() {
    return builtin::StrTypeId();
}

string StrLit::llvm_eval(std::ostream& os, int& local_var_count) {
    // return llvm_id;
    string len = std::to_string(lit->size()+1);
    return "getelementptr inbounds ([" + len + " x i8], [" + len + " x i8]* "
            + llvm_id + ", i32 0, i32 0)";
}

void StrLit::print(int lvl) {
    std::cout << string(lvl, '\t') << "str_lit: " << *lit << '\n';
}

// FuncCall::FuncCall(t_name* object, t_expressions* args)
//     : object(object), args(args) {  } //TODO ya está en el hpp

Id* FuncCall::exp_type() {
    //TODO las funciones no se pueden resolver con el Id porque eso apunta al mayor namespace
    // que contiene esa función pero puede haber otros debajo con el mismo nombre de función
    // y otros atributos :/.
    vector<Id*> signature(args->size());
    for (int i = 0; i < (int) args->size(); ++i) {
        signature[i] = (*args)[i]->exp_type();
    }
    Function* func = id->can_be_called(signature);
    assert(func != nullptr);
    if (func == nullptr) {
        //TODO error
    } 
    return func->type(); //TODO esto es para variables no funciones
}

string FuncCall::llvm_eval(std::ostream& os, int& local_var_count) {
    vector<Id*> signature(args->size());
    vector<string> exp_ress;
    for (int i = 0; i < (int) args->size(); ++i) {
        signature[i] = (*args)[i]->exp_type();
        exp_ress.push_back((*args)[i]->llvm_eval(os, local_var_count));
    }
    Function* func = id->can_be_called(signature);
    assert(func != nullptr);
    string ref = "%" + std::to_string(local_var_count++);
    os << "\t" << ref << " = call " << func->type()->llvm_type_name() << " @" << func->name()
       << "(";
    for (int i = 0; i < (int) args->size(); ++i) {
        if (i)
            os << ", ";
        os << signature[i]->llvm_type_name() << " " << exp_ress[i];
    }
    os << ")\n";
    return ref;
}

void FuncCall::print(int lvl) {
    std::cout << string(lvl, '\t') << "call to " << id->name() << "\n";
    for (auto arg : *args) {
        args->print(lvl+1);
    }
}

Id* UnaOp::exp_type() {
    return builtin::IntTypeId(); //TODO
}

string UnaOp::llvm_eval(std::ostream& os, int& local_var_count) {
    //TODO
    return "TODO";
}

void UnaOp::print(int lvl) {
    string tabs(lvl, '\t');
    std::cout << tabs << "UnaOp:\n";
    std::cout << tabs << "\t-\n";
    exp->print(lvl+1);
    std::cout << '\n';
}

Id* BinOp::exp_type() {
    //TODO resolver las funciones en una pasada
    Function* func = identifiers::GetId("operator+")->
                         can_be_called({lhs->exp_type(), rhs->exp_type()});
    assert(func != nullptr);
    return func->type(); //TODO
}

string BinOp::llvm_eval(std::ostream& os, int& local_var_count) {
    Function* func = identifiers::GetId("operator+")->
                         can_be_called({lhs->exp_type(), rhs->exp_type()});
    if (func == nullptr) {
        //TODO otro error
        semantic_error << "no existe el operador!\n";
        return "{UNAVAILABLE OPERATOR}";
    }
    string lhs_str = lhs->llvm_eval(os, local_var_count);
    string rhs_str = rhs->llvm_eval(os, local_var_count);
    return func->llvm_put_call(os, local_var_count, {&lhs_str, &rhs_str});
}

void BinOp::print(int lvl) {
    string tabs(lvl, '\t');
    std::cout << tabs << "t_binary_op:\n";
    lhs->print(lvl+1);
    std::cout << tabs << '\t' << (char) op << '\n';
    rhs->print(lvl+1);
    std::cout << '\n';
}

} // namespace ast

} // namespace compiler

