#include "identifiers.hpp"
#include <assert.h>

namespace AST {


t_id* t_id::named(const std::string& name) {
    t_id*& ptr = identifiers_look_up[name];
    if (ptr == nullptr) {
        ptr = new t_id(name);
        program_identifiers.push_back(ptr);
    }
    return ptr;
}

t_id::t_id(const std::string& name) : name_(name), obj_type_(ObjType::UNDECLARED) {  }

bool t_id::register_function(t_function* func) {
    switch (obj_type_) {
        case UNDECLARED:
            obj_type_ = FUNCTION;
            obj_data_.funcs = std::vector<t_function*>();
            // Deliberate fall-through
        case FUNCTION:
            if (can_be_called(func->signature()))
                return false;
            obj_data_.funcs.push_back(func);
            return true;
        default:
            return false;
    }
}

bool t_id::register_as_variable(t_type* type) {
    if (obj_type_ != UNDECLARED)
        return false;
    obj_type_ = VARIABLE;
    obj_data_.var = new t_var(type); //TODO considerar usar memoria estática
    return true;
}

bool t_id::register_as_constant(t_int_lit* val) {
    if (obj_type_ != UNDECLARED)
        return false;
    obj_type_ = CONSTANT;
    obj_data_.cons = new t_constant(val);
    return true;
}

bool t_id::register_as_type(t_type* type) {
    if (obj_type_ != UNDECLARED)
        return false;
    obj_type_ = TYPE;
    obj_data_.type = type;
    return true;
}

bool t_id::can_be_called(const std::vector<t_id*>& signature) {
    if (obj_type_ != FUNCTION)
        return false;
    for (auto func : obj_data_.funcs) {
        if (func->signature() == signature)
            return true;
    }
    return false;
}

t_type* t_id::exp_type() {
    assert(obj_type_ == VARIABLE);
    return obj_data_.var->type();
}

void t_id::print(int lvl) {
    std::cout << std::string(lvl, '\t') << "id: " << name_ << '\n';
}

}
