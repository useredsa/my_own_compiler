#include "id_resolution.hpp"

#include <algorithm>
#include <iomanip>

#include "builtin.hpp"
#include "identifiers.hpp"
#include "ast.hpp"
#include "log.hpp"
#include "statements.hpp"

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
                    semantic_log << "identifier " << name << "(" << info.ids[i]->scope_
                                 << ") couldn't be resolved\n";
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
            semantic_log << var->rtype().id->name() << " is not type!" << std::endl;
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
                    ast::RType& rtype = id->ref.funs[i]->rtype();
                    if (rtype.id->abstracts_ == kRedirected) {
                        rtype.id = rtype.id->ref.id;
                    }
                    if (not rtype.id->IsAType()) {
                        semantic_log << "Return type " << rtype.id->name()
                                     << " is not a type!\n";
                        rtype.ty = builtin::ErrorType();
                    } else {
                        rtype.ty = rtype.id->type();
                    }
                }
            }
        }
    }
}

ast::Fun* NameResolution::FromFunSig(Id* id, const std::vector<ast::Type*>& signature) {
    NameInfo& info = name_table[id->name()];
    size_t index = std::find(info.ids.begin(), info.ids.end(), id) - info.ids.begin();
    assert(index < info.ids.size());
    while (true) {
        if (info.ids[index]->abstracts_ == kRedirected) {
            id = info.ids[index];
            while (index >= 0 and info.ids[index] != id->ref.id) {
                assert (index != 0);
                --index;
            }
        }
        id = info.ids[index];
        if (id->abstracts_ != kFunctions) {
            semantic_log << id->name() << " is not a function!\n";
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
        while (index > 0 and info.ids[index]->scope_ != id->scope_->parent_) {
            --index;
        }
        if (info.ids[index]->scope_ != id->scope_->parent_) {
            break;
        }
    }
    return builtin::ErrorFun();
}

void NameResolution::Do() {
    FirstPass();
    SecondPass();
}

void NameResolution::operator()(ast::Prog* prog) {
    for (auto func : prog->funs) {
        (*this)(func);
    }
}

void NameResolution::operator()(ast::Fun* fun) {
    for (ast::Var* var : fun->var_dcls()) {
        (*this)(var);
    }
    (*this)(fun->stmts());
}

void NameResolution::operator()(ast::Var* var) {
    if (not std::holds_alternative<ast::NoExp*>(var->val())) {
        ast::Type* type = GetType(var->val());
        if (type != var->rtype().ty) {
            semantic_log << "Initialized constant " << var->id()->name()
                         << " with the wrong type " << var->rtype().ty->id()->name() << "\n";
        }
    }
}

void NameResolution::operator()(ast::Stmt& stmt) {
    std::visit(*this, stmt);
}

//NOLINTNEXTLINE(misc-unused-parameters): parameter is necessary variant visitor.
void NameResolution::operator()(ast::EmptyStmt* empty) {
}

void NameResolution::operator()(ast::CompStmt* stmts) {
    for (auto stmt : *stmts) {
        (*this)(stmt);
    }
}

void NameResolution::operator()(ast::Assig* assig) {
    ast::Type* lhs_type = GetType(assig->rvar);
    ast::Type* rhs_type = GetType(assig->exp);
    if (lhs_type != rhs_type) {
        semantic_log << "Cannot convert " << rhs_type->id()->name()
                     << " to " << lhs_type->id()->name() << " in assignment\n";
    }
    if (assig->rvar.var->is_constant()) {
        semantic_log << "Cannot assign to const variable "
                     << assig->rvar.var->id()->name() << "\n";
    }
}

void NameResolution::operator()(ast::IfStmt* if_stmt) {
    ast::Type* cond_type = GetType(if_stmt->exp);
    if (cond_type != builtin::IntTypeId()->ref.type) {
        semantic_log << "Incompatible type in if condition: expected "
                     << builtin::IntTypeId()->name() << '\n';
    }
    (*this)(if_stmt->stmt);
    (*this)(if_stmt->alt_stmt);
}

void NameResolution::operator()(ast::WhileStmt* while_stmt) {
    ast::Type* cond_type = GetType(while_stmt->exp);
    if (cond_type != builtin::IntTypeId()->ref.type) {
        semantic_log << "Incompatible type in while condition: expected "
                     << builtin::IntTypeId()->name() << '\n';
    }

    (*this)(while_stmt->stmt);
}

void NameResolution::operator()(ast::ForStmt* for_stmt) {
    ast::Type* ctrl_type = GetType(for_stmt->rvar);
    if (ctrl_type != builtin::IntTypeId()->ref.type) {
        semantic_log << "Incompatible type in for control variable: expected "
                     << builtin::IntTypeId()->name() << '\n';
    }

    ast::Type* init_type = GetType(for_stmt->start_exp);
    if (init_type != builtin::IntTypeId()->ref.type) {
        semantic_log << "Incompatible type in for initial expression: expected "
                     << builtin::IntTypeId()->name() << '\n';
    }

    ast::Type* final_type = GetType(for_stmt->end_exp);
    if (final_type != builtin::IntTypeId()->ref.type) {
        semantic_log << "Incompatible type in for final expression: expected "
                     << builtin::IntTypeId()->name() << '\n';
    }

    (*this)(for_stmt->stmt);
}

void NameResolution::operator()(ast::ReadStmt* read_stmt) {
    ast::Type* type;
    for (ast::RVar& rvar : read_stmt->rvars) {
        type = GetType(rvar);
        if (type != builtin::IntTypeId()->ref.type and
            type != builtin::StrTypeId()->ref.type) {
            semantic_log << "Incompatible type in read statement: "
                         << " unexpected " << type->id()->name()
                         << " expected " << builtin::IntTypeId()->name()
                         << " or " << builtin::StrTypeId()->name() << '\n';
        }
    }
}

void NameResolution::operator()(ast::WriteStmt* write_stmt) {
    ast::Type* type;
    for (ast::Exp& exp : write_stmt->exps) {
        type = GetType(exp);
        if (type != builtin::IntTypeId()->ref.type and
            type != builtin::StrTypeId()->ref.type) {
            semantic_log << "Incompatible type in write statement:"
                         << " unexpected " << type->id()->name()
                         << " expected " << builtin::IntTypeId()->name()
                         << " or " << builtin::StrTypeId()->name() << '\n';
        }
    }
}

ast::Type* NameResolution::GetType(ast::Exp& exp) {
    return std::visit(*this, exp);
}

ast::Type* NameResolution::GetType(ast::RVar& rvar) {
    return (*this)(rvar);
}

//NOLINTNEXTLINE(misc-unused-parameters): parameter is necessary variant visitor.
ast::Type* NameResolution::operator()(ast::NoExp* no_exp) {
    return builtin::ErrorType();
}

//NOLINTNEXTLINE(misc-unused-parameters): parameter is necessary variant visitor.
ast::Type* NameResolution::operator()(ast::IntLit* int_lit) {
    return builtin::IntTypeId()->ref.type;
}

//NOLINTNEXTLINE(misc-unused-parameters): parameter is necessary variant visitor.
ast::Type* NameResolution::operator()(ast::StrLit* str_lit) {
    return builtin::StrTypeId()->ref.type;
}

ast::Type* NameResolution::operator()(ast::FunCall* call) {
    std::vector<ast::Type*> args_types;
    for (ast::Exp& arg : call->args) {
        args_types.push_back(GetType(arg));
    }
    ast::Fun* fun = FromFunSig(call->rfun.id, args_types);
    if (fun == builtin::ErrorFun()) {
        semantic_log << "Invalid call to function "
                     << call->rfun.id->name() << '\n';
        return builtin::ErrorType();
    }
    call->rfun.fun = fun;
    
    return fun->rtype().ty;
}

ast::Type* NameResolution::operator()(ast::RVar& rvar) {
    if (rvar.id->abstracts_ == kRedirected) {
        rvar.id = rvar.id->ref.id;
    }

    if (not rvar.id->IsAVariable()) {
        semantic_log << rvar.id->name() << " is not a variable!\n";
        return builtin::ErrorType();
    }
    rvar.var = rvar.id->ref.var;

    return rvar.var->rtype().ty;
}

template<ast::UnaryOperators op>
ast::Type* NameResolution::operator()(ast::UnaOp<op>* una_op) {
    ast::Type* type = GetType(una_op->exp);
    una_op->rfun.fun = FromFunSig(una_op->rfun.id, {type});
    if (una_op->rfun.fun == builtin::ErrorFun()) {
        semantic_log << "Invalid use of operator " << static_cast<char>(op) << '\n';
        return builtin::ErrorType();
    }
    return una_op->rfun.fun->rtype().ty;
}

template<ast::BinaryOperators op>
ast::Type* NameResolution::operator()(ast::BinOp<op>* bin_op) {
    ast::Type* lhs_type = GetType(bin_op->lhs);
    ast::Type* rhs_type = GetType(bin_op->rhs);

    bin_op->rfun.fun = FromFunSig(bin_op->rfun.id, {lhs_type, rhs_type});
    if (bin_op->rfun.fun == builtin::ErrorFun()) {
        semantic_log << "Invalid use of operator " << static_cast<char>(op) << '\n';
        return builtin::ErrorType();
    }
    return bin_op->rfun.fun->rtype().ty;
}

template<typename T>
void NameResolution::operator()(std::vector<T> vec) {
    for (auto obj : vec) {
        (*this)(obj);
    }
}

} // namespace identifiers

} // namespace compiler
