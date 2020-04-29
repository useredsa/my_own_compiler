#include "identifiers.hpp"

#include <assert.h>

#include <iostream>
#include <cstring>

namespace AST {

t_id::t_id(const std::string& name) : name_(name), obj_type_(UNDECLARED) {  }

bool t_id::register_function(t_function* func) {
    switch (obj_type_) {
        case UNDECLARED:
            obj_type_ = FUNCTION;
            obj_data_.funcs = std::vector<t_function*>();
            // Deliberate fall-through
        case FUNCTION:
            if (can_be_called(func->signature()) != nullptr)
                return false;
            obj_data_.funcs.push_back(func);
            return true;
        default:
            return false;
    }
}

bool t_id::register_as_variable(t_id* type) {
    if (obj_type_ != UNDECLARED)
        return false;
    obj_type_ = VARIABLE;
    obj_data_.var = new t_var(name_, type); //TODO considerar usar memoria estática
    return true;
}

bool t_id::register_as_constant(t_int_lit* val) {
    if (obj_type_ != UNDECLARED)
        return false;
    obj_type_ = CONSTANT;
    obj_data_.cons = new t_constant(name_, val);
    return true;
}

bool t_id::register_as_type(t_type* type) {
    if (obj_type_ != UNDECLARED)
        return false;
    obj_type_ = TYPE;
    obj_data_.type = type;
    return true;
}

t_function* t_id::can_be_called(const std::vector<t_id*>& signature) {
    if (obj_type_ != FUNCTION)
        return nullptr;
    for (auto func : obj_data_.funcs) {
        if (func->signature() == signature)
            return func;
    }
    return nullptr;
}

t_id* t_id::exp_type() {
    assert(obj_type_ == VARIABLE);
    return obj_data_.var->type();
}

std::string t_id::llvm_eval(std::ostream& os, int& local_var_count) {
    if (obj_type_ == VARIABLE) {
        std::string ref = "%" + std::to_string(local_var_count++);
        os << "\t" << ref << " = " << "load "
           << exp_type()->llvm_type_name()
           << ", " << exp_type()->llvm_type_name() << "* %" << name_
           << ", align 4\n";
        return ref;
    }
    if (obj_type_ == CONSTANT) {
        //TODO
    }
    //TODO y otro error pero este debe comprobarse antes :/
    return "ERROR";
}

void t_id::llvm_var_alloca(std::ostream& os) {
    assert(obj_type_ == VARIABLE);
    os << "\t%" << name_ << " = alloca " << obj_data_.var->type()->llvm_type_name()
       << ", align 4\n";
}

void t_id::print(int lvl) {
    std::cout << std::string(lvl, '\t') << "id: " << name_ << '\n';
}

}  // namespace AST
