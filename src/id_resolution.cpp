#include "id_resolution.hpp"

#include <algorithm>
#include <iomanip>
#include "builtin.hpp"
#include "identifiers.hpp"
#include "ast.hpp"
#include "log.hpp"
#include "types.hpp"

namespace compiler {

namespace identifiers {

/*
 * @brief Binds NamedAbstractions who own a name (the name cannot be overloaded).
 */
void NameResolution::FirstPass() {
    for (auto& [name, info] : name_table) {
        // Sort names according to namescope depth
        sort(info.ids.begin(), info.ids.end(), [](Id* lhs, Id* rhs) {
            return lhs->namescope()->depth() < rhs->namescope()->depth();
        });

        for (size_t i = 0; i < info.ids.size(); ++i) {
            // semantic_log << "identifier " << info.ids[i]->name()
            //              << " from scope " << info.ids[i]->scope_ << "\n";
            if (info.ids[i]->abstracts_ == kUnresolved) {
                NameScope* parent_scope = info.ids[i]->scope_->parent_;
                for (size_t j = 0; j < i; ++j) {
                    if (info.ids[i]->scope_ == parent_scope) {
                        info.ids[i]->abstracts_ = kRedirected;
                        info.ids[i]->ref.id = info.ids[j]->abstracts_ == kRedirected?
                                              info.ids[j]->ref.id : info.ids[j];
                    }
                }
                if (info.ids[i]->abstracts_ == kUnresolved) {
                    semantic_log << "identifier " << name << " couldn't be resolved\n";
                }
            }
        }
    }
}

void NameResolution::SecondPass() {
    // Update references to the type of each variable
    for (ast::Var* var : ast::program_vars) {
        // We add this checks to allow extra semantic checks even if we don't
        // generate llvm code.
        if (var->rtype().id->abstracts_ == kUnresolved) {
            std::cerr << var->rtype().id->name() << " is not type!" << std::endl;
            var->rtype().id->abstracts_ = kRedirected;
            var->rtype().id->ref.id = builtin::IntTypeId();
        }
        var->rtype().ty = var->rtype().id->type();
    }
    // Check for repeated functions (once we can check the actual types of their arguments)
    for (auto& [name, info] : name_table) {
        for (Id* id : info.ids) {
            if (id->abstracts_ == kFunctions) {
                for (size_t i = 0; i < id->ref.funs.size(); ++i) {
                    for (size_t j = 0; j < i; ++j) {
                        if (std::equal(
                            id->ref.funs[i]->args().begin(), id->ref.funs[i]->args().end(),
                            id->ref.funs[j]->args().begin(), id->ref.funs[j]->args().end(),
                            [](ast::Var* lhs, ast::Var* rhs) {
                                return lhs->rtype().ty == rhs->rtype().ty;
                            })) {
                            semantic_log << "Two identical functions " << std::quoted(name)
                                         << " at the same scope\n";
                        }
                    }
                }
            }
        }
    }
}

ast::Fun* NameResolution::CallToFun(Id* id, const std::vector<ast::Type*>& signature) {
    NameInfo& info = name_table[id->name()];
    size_t index = std::find(info.ids.begin(), info.ids.end(), id) - info.ids.begin();
    assert(index < info.ids.size());
    while (index >= 0) {
        if (info.ids[index]->abstracts_ == kRedirected) {
            id = info.ids[index];
            while (index >= 0 and info.ids[index] != id->ref.id) {
                --index;
            }
            assert(index >= 0);
        }
        id = info.ids[index];
        if (id->abstracts_ != kFunctions) {
            break;
        }
        for (ast::Fun* fun : id->ref.funs) {
            if (std::equal(fun->args().begin(), fun->args().end(),
                           signature.begin(), signature.end(),
                           [](ast::Var* var, ast::Type* type) {
                               return var->rtype().ty == type;
                           })) {
                return fun;
            }
        }
        while (index >= 0 and info.ids[index]->scope_ != id->scope_->parent_) {
            --index;
        }
    }
    semantic_log << "Invalid call to function " << id->name() << "\n";
    return builtin::ErrorFun();
}

void NameResolution::Do() {
    FirstPass();
    SecondPass();
}


} // namespace identifiers

} // namespace compiler
