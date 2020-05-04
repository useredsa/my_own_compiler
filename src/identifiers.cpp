#include "identifiers.hpp"

#include <cassert>
#include <stack>
#include <unordered_map>
#include "ast.hpp"

namespace compiler {

namespace identifiers {

using std::vector;
using std::string;
using std::pair;
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
            if (can_be_called(func->args()) != nullptr) {
                return false;
            }
            ref.funs.push_back(func);
            return true;
        default:
            return false;
    }
}

bool Id::RegisterAsVariable(Var* var) {
    if (abstracts_ != kUnresolved) {
        return false;
    }
    abstracts_ = kVariable;
    ref.var = var;
    return true;
}

bool Id::RegisterAsType(Type* type) {
    if (abstracts_ != kUnresolved) {
        return false;
    }
    abstracts_ = kType;
    ref.type = type;
    return true;
}

Fun* Id::can_be_called(const std::vector<Var*>& signature) {
    if (abstracts_ != kFunctions) {
        return nullptr;
    }
    for (Fun* fun : ref.funs) {
        if (signature.size() != fun->args().size()) {
            continue;
        }
        for (size_t i = 0; i < signature.size(); ++i) {
            if (signature[i]->rtype().ty != fun->args()[i]->rtype().ty) //TODO comprobación errónea
                continue;
        }
        return fun;
    }
    return nullptr;
}

struct NameInfo {
    std::stack<pair<size_t, Id*>> active_declarations;
    vector<Id*> ids;
};

vector<NameScope*> program_scopes;
vector<NameScope*> active_scopes; // Behaves as stack
vector<size_t> acronological_scopes;
std::unordered_map<string, NameInfo> name_table;


NameScope* current_name_scope() {
    return active_scopes.back();
}

NameScope* AddNameScope(NameScopeType type) {
    auto* scope = new NameScope{(active_scopes.empty()? nullptr : active_scopes.back()),
                                     type};
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

/**
 * Declares an identifier in an active scope
 */
Id* NewId(string&& name, NameScope* scope, size_t scope_pos) {
    NameInfo& info = name_table[name];
    auto* id = new Id(scope, std::move(name));
    info.ids.push_back(id);
    info.active_declarations.emplace(scope_pos, id);
    return id;
}

Id* NewId(string&& name) {
    return NewId(std::move(name), active_scopes.back(), active_scopes.size()-1);
}

Id* GetId(string&& name) {
    NameInfo& info = name_table[name];
    size_t last_acronological = acronological_scopes.back();
    while (not info.active_declarations.empty()) {
        auto [pos, id] = info.active_declarations.top();
        // If the last declaration of a variable is still active
        if (pos < active_scopes.size() and id->namescope() == active_scopes[pos]) {
            // check if belongs to a name scope under the
            // last active acronological scope
            if (pos >= last_acronological) {
                return id;
            }
            break;
        }
        info.active_declarations.pop();
    }
    return NewId(std::move(name), active_scopes[last_acronological], last_acronological);
}

} // namespace identifiers

} // namespace compiler
