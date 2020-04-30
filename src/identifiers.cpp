#include "identifiers.hpp"

#include "function.hpp"
#include <assert.h>
#include <unordered_map>
#include <stack>

template<typename T>
using vector = std::vector<T>;
using string = std::string;
template<typename T, typename U>
using pair = std::pair<T, U>;

namespace compiler {

namespace ast {

bool Id::RegisterFunction(Function* func) {
    switch (abstracts_) {
        case kUnresolved:
            abstracts_ = kFunctions;
            ref.funcs = std::vector<Function*>();
            // Deliberate fall-through
        case kFunctions:
            if (can_be_called(func->signature()) != nullptr)
                return false;
            ref.funcs.push_back(func);
            return true;
        default:
            return false;
    }
}

bool Id::RegisterAsVariable(Id* type) {
    if (abstracts_ != kUnresolved)
        return false;
    abstracts_ = kVariable;
    ref.var = new Var(this, type);
    return true;
}

bool Id::RegisterAsConstant(IntLit* val) {
    if (abstracts_ != kUnresolved)
        return false;
    abstracts_ = kConstant;
    ref.cons = new Constant(this, val);
    return true;
}

bool Id::RegisterAsType(Type* type) {
    if (abstracts_ != kUnresolved)
        return false;
    abstracts_ = kType;
    ref.type = type;
    return true;
}

Function* Id::can_be_called(const std::vector<Id*>& signature) {
    if (abstracts_ != kFunctions)
        return nullptr;
    for (auto func : ref.funcs) {
        if (func->signature() == signature)
            return func;
    }
    return nullptr;
}

Id* Id::exp_type() {
    assert(abstracts_ == kVariable);
    return ref.var->type;
}

std::string Id::llvm_eval(std::ostream& os, int& local_var_count) {
    if (abstracts_ == kVariable) {
        std::string ref = "%" + std::to_string(local_var_count++);
        os << "\t" << ref << " = " << "load "
           << exp_type()->llvm_type_name()
           << ", " << exp_type()->llvm_type_name() << "* %" << name_
           << ", align 4\n";
        return ref;
    }
    if (abstracts_ == kConstant) {
        //TODO
    }
    //TODO y otro error pero este debe comprobarse antes :/
    return "ERROR";
}

void Id::llvm_var_alloca(std::ostream& os) {
    assert(abstracts_ == kVariable);
    os << "\t%" << name_ << " = alloca " << ref.var->type->llvm_type_name()
       << ", align 4\n";
}

void Id::print(int lvl) {
    std::cout << std::string(lvl, '\t') << "id: " << name_ << '\n';
}

} // namespace ast



namespace identifiers {

using ast::Id;
using ast::NameScope;
using ast::NameScopeType;
using ast::NameScopeType::kAcronological;

struct NameInfo {
    std::stack<pair<int, Id*>> active_declarations;
    vector<Id*> ids;
};

vector<NameScope*> program_scopes;
vector<NameScope*> active_scopes; // Behaves as stack
vector<int> acronological_scopes;
std::unordered_map<string, NameInfo> name_table;


NameScope* current_name_scope() {
    return active_scopes.back();
}

NameScope* AddNameScope(NameScopeType type) {
    NameScope* scope = new NameScope{(active_scopes.empty()? nullptr : active_scopes.back()),
                                     type};
    program_scopes.push_back(scope);
    if (type == kAcronological)
        acronological_scopes.push_back(active_scopes.size());
    active_scopes.push_back(scope);
    return scope;
}

void AbandonCurrentNameScope() {
    assert(active_scopes.size() > 1);
    NameScope* popped = active_scopes.back();
    active_scopes.pop_back();
    if (acronological_scopes.back() >= (int) active_scopes.size())
        acronological_scopes.pop_back();
}

/**
 * Declares an identifier in an active scope
 */
Id* NewId(const string& name, NameScope* scope, int scope_pos) {
    NameInfo& info = name_table[name];
    Id* id = new Id(scope, name);
    info.ids.push_back(id);
    info.active_declarations.emplace(scope_pos, id);
    return id;
}

Id* NewId(const string& name) {
    return NewId(name, active_scopes.back(), (int) active_scopes.size()-1);
}

Id* GetId(const string& name) {
    NameInfo& info = name_table[name];
    int last_acronological = acronological_scopes.back();
    while (not info.active_declarations.empty()) {
        auto [pos, id] = info.active_declarations.top();
        // If the last declaration of a variable is still active
        if (pos < (int) active_scopes.size() and id->namescope() == active_scopes[pos]) {
            // check if belongs to a name scope under the
            // last active acronological scope
            if (pos >= last_acronological)
                return id;
            break;
        }
        info.active_declarations.pop();
    }
    return NewId(name, active_scopes[last_acronological], last_acronological);
}

} // namespace identifiers

} // namespace compiler
