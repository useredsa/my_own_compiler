#include "namespace.hpp"

#include "identifiers.hpp"

namespace AST {

t_namespace* t_namespace::current_namesp_ = new t_namespace();

t_id* t_namespace::new_id(const string& name) {
    t_id*& ptr = current_namesp_->identifiers[name];
    if (ptr != nullptr) {
        //TODO aquí tenemos un error
    }
    ptr = new t_id(name);
    return ptr;
}

t_id* t_namespace::get_id(const string& name) {
    auto it = current_namesp_->identifiers.find(name);
    if (it != current_namesp_->identifiers.end())
        return it->second;
    //TODO se puede imprimir un error aquí o
    // se puede devolver de alguna manera que
    // el nombre no estaba en el namespace.
    t_id*& ptr = current_namesp_->error_ids[name];
    if (ptr == nullptr)
        ptr = new t_id(name);
    return ptr;
}

t_unresolved_name* t_namespace::pending_id(const string& name) {
    //TODO explicar con comentarios
    auto it = current_namesp_->identifiers.find(name);
    if (it != current_namesp_->identifiers.end()) {
        t_unresolved_name*& ptr = current_namesp_->resolved_names[name];
        if (ptr == nullptr)
            ptr = new t_unresolved_name(it->second);
        return ptr;
    }
    t_unresolved_name*& ptr = current_namesp_->unresolved_names[name];
    if (ptr == nullptr)
        ptr = new t_unresolved_name(name);
    return ptr;
}

std::vector<t_namespace*> program_namespaces;

t_namespace::t_namespace(t_namespace* parent) :
    parent(parent == nullptr? this : parent),
    identifiers(),
    unresolved_names(),
    resolved_names(),
    error_ids() {
    program_namespaces.push_back(this);
}

t_namespace* t_namespace::create() {
    return new t_namespace(current_namesp_);
}

void t_namespace::switch_namespace(t_namespace* namesp) {
    current_namesp_ = namesp;
}

void t_namespace::switch_to_parent_namespace() {
    current_namesp_ = current_namesp_->parent;
}

void resolve_names() {
    for (auto namesp : program_namespaces) {
        for (auto unres_name : namesp->unresolved_names) {
            t_id* id = namesp->get_id
        }
    }
}

} // namespace AST
