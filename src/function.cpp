#include "function.hpp"

namespace AST {

void
t_declarations::add_constants(const std::vector<std::pair<t_id*, t_int_lit*>>&  cons) {
    for (auto [id, val] : cons) {
        if (!id->register_as_constant(val)) {
            //TODO std::cerr << "ERROR..."
        }
        constants.push_back(id);
    }
}

void t_declarations::add_identifiers(const std::vector<t_id*>& ids, t_id* type) {
    for (t_id* id : ids) {
        if (!id->register_as_variable(type)) {
            std::cerr << "ERROR: identifier " <<  id->name() << " already in use. Unavailable to define as a variable\n";
            //TODO habrá que registrar los errores para llevar cuenta
        }
        variables.push_back(id);
    }
}

void t_declarations::print(int lvl) {
    std::string tabs(lvl, '\t');
    std::cout << tabs << "declarations:\n";
    std::cout << tabs << "\tconstants\n";
    for (auto c : constants) {
        c->print(lvl+2);
    }
    std::cout << tabs << "\tvariables\n";
    for (auto v : variables) {
        v->print(lvl+2);
    }
}

t_function::t_function(t_id* type,
               t_id* name,
               const std::vector<std::pair<t_id*, t_id*>>& args,
               t_declarations* decls,
               t_statements* statements) :
               type_(type),
               args_(args.size()),
               signature_(args.size()),
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
