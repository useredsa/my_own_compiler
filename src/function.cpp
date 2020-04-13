#include "function.hpp"

namespace AST {

void
t_declarations::add_constants(const std::vector<std::pair<t_id*, t_int_lit*>>&  cons) {
    for (auto [id, val] : cons) {
        if (!id->register_as_constant(val)) {
            //TODO std::cerr << "ERROR..."
        }
        constants_.push_back(id);
    }
}

void t_declarations::add_identifiers(const std::vector<t_id*>& ids, t_id* type) {
    for (t_id* id : ids) {
        if (!id->register_as_variable(type)) {
            std::cerr << "ERROR: identifier " <<  id->name() << " already in use. Unavailable to define as a variable\n";
            //TODO habrá que registrar los errores para llevar cuenta
        }
        variables_.push_back(id);
    }
}

void t_declarations::llvm_put_constants(std::ostream& os) {
    
}

void t_declarations::llvm_put_variables(std::ostream& os) {
    for (t_id* id : variables_) {
        if (not id->is_a_variable()) {
            std::cerr << "ERROR: not a variable!\n"; //TODO
            continue;
        }
        id->llvm_var_alloca(os);
    }
}

void t_declarations::print(int lvl) {
    std::string tabs(lvl, '\t');
    std::cout << tabs << "declarations:\n";
    std::cout << tabs << "\tconstants\n";
    for (auto c : constants_) {
        c->print(lvl+2);
    }
    std::cout << tabs << "\tvariables\n";
    for (auto v : variables_) {
        v->print(lvl+2);
    }
}

t_function::t_function(t_id* type,
                       t_id* name,
                       const std::vector<std::pair<t_id*, t_id*>>& args,
                       t_declarations* decls,
                       t_statements* statements) :
                       type_(type),
                       name_(name->name()),
                       args_(),
                       signature_(),
                       declarations_(decls),
                       statements_(statements)
{
    for (auto [type, id] : args) {
        signature_.push_back(type);
        args_.push_back(id);
    }
    // Important: Register after construction is finished
    if (!name->register_function(this)) {
        //TODO std::cerr << "ERROR: the function cannot be named " << name << " a variable or a function with the same signature is registered with that name\n";
    }
}

void t_function::llvm_put(std::ostream& os) {
    declarations_->llvm_put_constants(os);
    os << ";Function\n";
    os << "define i32 @" << "main" << "() {\n"; //TODO include signature
    int local_var_count = 1;
    declarations_->llvm_put_variables(os);
    for (auto st : *statements_) {
        st->llvm_put(os, local_var_count);
    }
    os << "\tret i32 0\n"; //TODO return type!
    os << "}\n\n";
}

//TODO considerar usar using std::string porque es horrible escribir estos nombres
std::string t_function::llvm_put_call(std::ostream& os,
                                      int& local_var_count,
                                      std::vector<std::string*> params) {
    return "TODO";
}

void t_function::print(int lvl) {
    std::string tabs(lvl, '\t');
    std::cout << "function " /*<< id_data[id]*/ << '\n';
    std::cout << "return type:\n";
    type_->print(lvl+1);
    std::cout << "\n\tsignature:\n";
    // for (auto arg : args_) {
    //     arg->print(lvl+2);
    // } //TODO
    //cout << "arguments type:\n";
    for (t_id* id : signature_) {
        id->print(lvl+1);
    }
    statements_->print(lvl+1);
}

}
