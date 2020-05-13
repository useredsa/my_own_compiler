#include "identifiers.hpp"

#include "ast.hpp"
#include "log.hpp"

namespace compiler {

namespace identifiers {

using std::vector;
using std::string;
using ast::Type;
using ast::Var;
using ast::Fun;

bool Id::RegisterFunction(Fun* func) {
    switch (abstracts_) {
        case kUnresolved:
            abstracts_ = kFunctions;
            ref.funs = std::vector<Fun*>();
            // Deliberate fall-through
        case kFunctions:
            ref.funs.push_back(func);
            return true;
        default:
        semantic_log << "Redefinition of " << name_ << " (as a function)\n";
            return false;
    }
}

bool Id::RegisterAsVariable(Var* var) {
    if (abstracts_ != kUnresolved) {
        semantic_log << "Redefinition of " << name_ << " (as a variable)\n";
        return false;
    }
    abstracts_ = kVariable;
    ref.var = var;
    return true;
}

bool Id::RegisterAsType(Type* type) {
    if (abstracts_ != kUnresolved) {
        semantic_log << "Redefinition of " << name_ << " (as a type)\n";
        return false;
    }
    abstracts_ = kType;
    ref.type = type;
    return true;
}









NameScope::NameScope(NameScopeType type, NameScope* parent)
    : parent_(parent), type_(type), depth_(parent == nullptr? 0 : parent->depth_+1) {
    
}

vector<NameScope*> program_scopes;
vector<NameScope*> active_scopes; // Behaves as stack
vector<size_t> acronological_scopes;
std::unordered_map<string, NameInfo> name_table;


inline NameScope* current_name_scope() {
    return active_scopes.back();
}

NameScope* AddNameScope(NameScopeType type) {
    auto* scope = new NameScope(type,
                                (active_scopes.empty()? nullptr : current_name_scope()));
    program_scopes.push_back(scope);
    if (type == kAcronological) {
        acronological_scopes.push_back(active_scopes.size());
    }
    active_scopes.push_back(scope);
    return scope;
}

void AbandonCurrentNameScope() {
    assert(active_scopes.size() > (size_t) 1);
    active_scopes.pop_back();
    if (acronological_scopes.back() >= active_scopes.size()) {
        acronological_scopes.pop_back();
    }
}

Id* pop_unactive(NameInfo* info) {
    assert(info != nullptr);
    size_t last_acronological = acronological_scopes.back();
    while (not info->active_declarations.empty()) {
        auto [pos, id] = info->active_declarations.top();
        // If the last declaration of a variable is still active
        if (pos < active_scopes.size() and id->namescope() == active_scopes[pos]) {
            // check if belongs to a name scope under the
            // last active acronological scope
            if (pos >= last_acronological) {
                return id;
            }
            break;
        }
        info->active_declarations.pop();
    }
    return nullptr;
}

/**
 * Declares a new (it is known to be new) identifier in an active scope
 */
Id* NewId(string&& name, NameScope* scope, size_t scope_pos) {
    NameInfo& info = name_table[name];
    auto* id = new Id(scope, std::move(name));
    info.ids.push_back(id);
    info.active_declarations.emplace(scope_pos, id);
    return id;
}

Id* NewId(string&& name) {
    NameInfo& info = name_table[name];
    if (Id* ptr = pop_unactive(&info); ptr and
        ptr->namescope() == current_name_scope()) {
        return ptr;
    }
    return NewId(std::move(name), current_name_scope(), active_scopes.size()-1);
}

Id* GetId(string&& name) {
    NameInfo& info = name_table[name];
    if (Id* ptr = pop_unactive(&info); ptr) {
        return ptr;
    }
    size_t last_acronological = acronological_scopes.back();
    return NewId(std::move(name), active_scopes[last_acronological], last_acronological);
}

} // namespace identifiers

} // namespace compiler
