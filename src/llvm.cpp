#include "llvm.hpp"

#include "statements.hpp"
#include "types.hpp"

namespace compiler {

using std::vector;
using std::string;
using identifiers::Id;
using ast::RType;
using ast::RVar;
using ast::RFun;
using ast::Prog;
using ast::Type;
using ast::Var;
using ast::Fun;
using ast::Stmt;
using ast::EmptyStmt;
using ast::CompStmt;
using ast::Assig;
using ast::IfStmt;
using ast::WhileStmt;
using ast::ForStmt;
using ast::ReadStmt;
using ast::WriteStmt;
using ast::Exp;
using ast::NoExp;
using ast::UnaOp;
using ast::BinOp;
using ast::IntLit;
using ast::StrLit;
using ast::FunCall;
using builtin::IntTypeId;
using builtin::StrTypeId;

namespace llvm {

inline string LocVar(const string& name) {
    return "%" + name;
}

inline string LocVar(int num) {
    return "%" + std::to_string(num);
}

/**
 * @brief Allocates a type with standard alignment in the local
 * variable with name var_name
 */
inline void Translator::EasyAlloca(Var* var) {
    os << "\t%" << var->id()->name() << " = alloca " << var->rtype().ty->llvm_name()
       << ", align " << var->rtype().ty->def_alignment() << "\n";
}

/**
 * @brief Stores a variable val in the location given by var.
 * The variable must be of type ty and the location of type ty*.
 */
inline void Translator::EasyStore(const string& val, Var* var) {
    os << "\tstore " << var->rtype().ty->llvm_name() << " " << val << ", "
       << var->rtype().ty->llvm_name() << "* %" << var->id()->name() << ", align "
       << var->rtype().ty->def_alignment() << "\n";
}

inline void PutUselessOp(std::ostream& os, int& local_var_count) {
    os << "\t%" << local_var_count++ << " = add nsw i32 0, 0 ; inserted-but-not-used\n";
}

void Translator::Output(Prog* prog) {
    // Include C stdin library
    os << "; ModuleID = '<stdin>'\n";
    // I'm using the name of the program here. We could use the name of the file.
    os << "source_filename = " << std::quoted(prog->name) << "\n";
    // Compilation target
    os << "target datalayout = \"e-m:e-i64:64-f80:128-n8:16:32:64-S128\"\n";
    os << "target triple = \"x86_64-pc-linux-gnu\"\n";
    os << "\n";

    // Global variables: allocate str literals
    LlvmPutStrLits();
    os << "\n\n";

    // Prog functions
    for (ast::Fun* func : prog->funs) {
        Output(func);
    }

    // C stdin library used functions
    os << "declare i32 @__isoc99_scanf(i8*, ...)\n";
    os << "declare i32 @printf(i8*, ...)\n";
    os << "declare noalias i8* @malloc(i64)\n";
    os << "declare i64 @strlen(i8*)\n";
    os << "declare i8* @strcpy(i8*, i8*)\n";
}

void Translator::Output(Fun* fun) {
    os << "; Function\n";
    os << "define " << fun->rtype().ty->llvm_name() << " @" << fun->llvm_name() << "(";
    const std::vector<Var*>& args = fun->args();
    for (size_t i = 0; i < args.size(); ++i) {
        if (i) os << ", ";
        os << args[i]->rtype().ty->llvm_name();
    }
    os << ") {\n";

    // Allocate the space for the function arguments, ...
    for (size_t i = 0; i < args.size(); ++i) {
        EasyAlloca(args[i]);
    }
    // ...variables, and...
    for (Var* var : fun->var_dcls()) {
        EasyAlloca(var);
    }
    // ...store in the value
    for (size_t i = 0; i < args.size(); ++i) {
        EasyStore(LocVar(i), args[i]);
    }
    for (Var* var : fun->var_dcls()) {
        if (!std::holds_alternative<NoExp*>(var->val())) {
            ComputedExp exp = Eval(var->val());
            EasyStore(exp.val, var);
        }
    }

    // The local variables counter starts at %1 for empty functions,
    // %n+1 for functions with n arguments (due to the previous allocas).
    local_var_count = 1+args.size();
    for (const Stmt& stmt : fun->stmts()) {
        Output(stmt);
    }


    const string& tyname = fun->rtype().ty->llvm_name();
    Var* var = fun->var_dcls().back();
    os << "\t%" << local_var_count++ << " = load " << tyname << ", " << tyname << "* %"
       << var->id()->name() << ", align " << fun->rtype().ty->def_alignment() << "\n";
    if (var->rtype().ty == StrTypeId()->type()) {
        string len = "%" + std::to_string(local_var_count++);
        os << "\t" << len << " = call i64 @strlen(i8* %" << local_var_count-2 << ")\n";
        os << "\t%" << local_var_count++ << " = add nsw i64 " << len << ", 1\n";
        string ref = "%" + std::to_string(local_var_count++);
        os << "\t" << ref << " = call noalias i8* @malloc(i64 %" << local_var_count-2
           << ")\n";
        os << "\t%" << local_var_count << " = call i8* @strcpy(i8* " << ref << ", i8* %"
           << local_var_count-4 << ")\n";
        local_var_count++;
    }
    os << "\tret " << tyname << " %" << local_var_count-1 << "\n";
    os << "}\n\n";
}

inline void Translator::Output(Stmt stmt) {
    std::visit(*this, stmt);
}


/******* Statements *******/

//NOLINTNEXTLINE(misc-unused-parameters): parameter is necessary variant visitor.
void Translator::operator()(EmptyStmt* empty_stmt) {

}

void Translator::operator()(CompStmt* cmp_stmt) {
    for (const Stmt& stmt : *cmp_stmt) {
        Output(stmt);
    }
}

void Translator::operator()(Assig* assig) {
    ComputedExp exp = Eval(assig->exp);
    EasyStore(exp.val, assig->rvar.var);
}

//NOTE 1: It's important to note that every block of continuous operations in
//        llvm ends with a special action. Either ret, jmp, or similar.

void Translator::operator()(IfStmt* if_stmt) {
    // Refer to NOTE 1.
    //IMPROVEMENT: We could assign more meaningful numbers to the labels
    int label_num = local_var_count;
    ComputedExp cond_exp = Eval(if_stmt->exp);
    os << "\t%" << local_var_count << " = icmp ne i32 " << cond_exp.val << ", 0\n";
    os << "\tbr i1 %" << local_var_count++ << ", label %then" << label_num << ", label %"
       << (std::holds_alternative<EmptyStmt*>(if_stmt->alt_stmt)? "fi" : "else")
       << label_num << "\n";
    os << "then" << label_num << ":\n";
    Output(if_stmt->stmt);
    os << "\tbr label %fi" << label_num << "\n";  
    if (!std::holds_alternative<EmptyStmt*>(if_stmt->alt_stmt)) {
        os << "else" << label_num << ":\n";
        Output(if_stmt->alt_stmt);
        os << "\tbr label %fi" << label_num << "\n";
    }
    os << "fi" << label_num<< ":\n";
}

void Translator::operator()(WhileStmt* while_stmt) {
    int label_num = local_var_count;
    os << "\tbr label %whilecomp" << label_num << "\n";
    os << "whilecomp" << label_num << ":\n";
    ComputedExp cond_exp = Eval(while_stmt->exp);
    os << "\t%" << local_var_count << " = icmp ne i32 " << cond_exp.val << ", 0\n";
    os << "\tbr i1 %" << local_var_count++
       << ", label %whileloop" << label_num
       << ", label %afterwhile" << label_num << "\n";
    os << "whileloop" << label_num << ":\n";
    Output(while_stmt->stmt);
    os << "\tbr label %whilecomp" << label_num << "\n";
    os << "afterwhile" << label_num << ":\n";
}

void Translator::operator()(ForStmt* for_stmt) {
    int label_num = local_var_count;
    ComputedExp start_exp = Eval(for_stmt->start_exp);
    EasyStore(start_exp.val, for_stmt->rvar.var);
    os << "\tbr label %forcomp" << label_num << "\n";

    Exp aux_id_exp = for_stmt->rvar;
    // Loop until id == end
    os << "forcomp" << label_num << ":\n";
    ComputedExp ctrl_id_exp = Eval(aux_id_exp);
    ComputedExp end_exp = Eval(for_stmt->end_exp);
    os << "\t%" << local_var_count << " = icmp ne " << IntTypeId()->type()->llvm_name()
       << " " << ctrl_id_exp.val << ", " << end_exp.val << "\n";
    os << "\tbr i1 %" << local_var_count++ << ", label %forloop" << label_num
       << ", label %afterfor" << label_num << "\n";
    // for body
    os << "forloop" << label_num << ":\n";
    Output(for_stmt->stmt);

    // increment id
    os << "\t%" << local_var_count << " = add nsw i32 " << ctrl_id_exp.val << ", 1\n";
    EasyStore(LocVar(local_var_count++), for_stmt->rvar.var);
    os << "\tbr label %forcomp" << label_num << "\n";
    os << "afterfor" << label_num << ":\n";
}

void Translator::operator()(ReadStmt* read_stmt) {
    for (RVar rvar : read_stmt->rvars) {
        os << "\t%" << local_var_count++ << " = call i32 (i8*, ...) "
           << "@__isoc99_scanf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* ";
        if (rvar.var->rtype().ty == IntTypeId()->type()) {
            os << "@.io.int";
        } else if (rvar.var->rtype().ty == StrTypeId()->type()) {
            os << "@.io.str";
        } else {
            internal_log << "Unexpected type in ReadStmt during llvm translation!"
                         << std::endl;
            exit(-1);
        }
        os << ", i32 0, i32 0), " << rvar.var->rtype().ty->llvm_name() << "* %"
           << rvar.var->id()->name() << ")\n";
    }
}

void Translator::operator()(WriteStmt* write_stmt) {
    for (const Exp& exp : write_stmt->exps) {
        ComputedExp eval = Eval(exp);
        os << "\t%" << local_var_count++ << " = call i32 (i8*, ...) @printf(i8* "
           << "getelementptr inbounds ([3 x i8], [3 x i8]* ";
        if (eval.type == IntTypeId()->type()) {
            os << "@.io.int";
        } else if (eval.type == StrTypeId()->type()) {
            os << "@.io.str";
        } else {
            internal_log << "Unexpected type in ReadStmt during llvm translation!"
                         << std::endl;
            exit(-1);
        }
        os << ", i32 0, i32 0), " << eval.type->llvm_name() << " " << eval.val << ")\n";
    }
}



/******* Expressions *******/

/**
 * NoExp is when you want to represent the absence of expression.
 * It shall not be evaluated!
 */
inline ComputedExp Translator::operator()(NoExp* no_exp) {
    assert(false);
}

inline ComputedExp Translator::operator()(IntLit* int_lit) {
    return {IntTypeId()->type(), std::to_string(int_lit->lit)};
}

ComputedExp Translator::operator()(StrLit* str_lit) {
    string len = std::to_string(str_lit->lit->size()+1);
    return {StrTypeId()->type(),
            "getelementptr inbounds ([" + len + " x i8], [" + len + " x i8]* "
            + str_lit->llvm_id + ", i32 0, i32 0)"
           };
}

ComputedExp Translator::operator()(RVar rvar) {
    string ref = LocVar(local_var_count++);
    Type* type = rvar.var->rtype().ty;
    os << "\t" << ref << " = load " << type->llvm_name() << ", "
        << type->llvm_name() << "* %" << rvar.var->id()->name() << ", align "
        << type->def_alignment() << "\n";
    return {type, ref};
}

template<ast::UnaryOperators op>
ComputedExp Translator::operator()(UnaOp<op>* una_op) {
    ComputedExp exp = Eval(una_op->exp);
    string ref = una_op->rfun.fun->
                   llvm_put_call(os, local_var_count, {&exp});
    return {una_op->rfun.fun->rtype().ty, ref};
}

template<ast::BinaryOperators op>
ComputedExp Translator::operator()(BinOp<op>* bin_op) {
    ComputedExp lhs_exp = Eval(bin_op->lhs);
    ComputedExp rhs_exp = Eval(bin_op->rhs);
    string ref = bin_op->rfun.fun->
                   llvm_put_call(os, local_var_count, {&lhs_exp, &rhs_exp});
    return {bin_op->rfun.fun->rtype().ty, ref};
}

ComputedExp Translator::operator()(FunCall* fun_call) {
    ComputedExp exps[fun_call->args.size()];
    vector<ComputedExp*> args(fun_call->args.size());
    for (size_t i = 0; i < fun_call->args.size(); ++i) {
        exps[i] = Eval(fun_call->args[i]);
        args[i] = &exps[i];
    }
    string ref = fun_call->rfun.fun->llvm_put_call(os, local_var_count, args);
    return {fun_call->rfun.fun->rtype().ty, ref};
}

inline ComputedExp Translator::Eval(Exp exp) {
    return std::visit(*this, exp);
}

} // namespace llvm

namespace ast {

std::string Fun::llvm_name() const {
    if (id_->name() == ".operator+") {
        return ".operatorPLUS";
    } else if (id_->name() == ".operator-") {
        return ".operatorMINUS";
    } else if (id_->name() == ".operator*") {
        return ".operatorASTERISK";
    } else if (id_->name() == ".operator/") {
        return ".operatorSLASH";
    }
    return id_->name();
}

std::string Fun::llvm_put_call(std::ostream& os,
                               int& local_var_count,
                               const vector<llvm::ComputedExp*>& params) {
    std::string ref = llvm::LocVar(local_var_count++);
    os << "\t" << ref << " = call " << rtype().ty->llvm_name() << " @" << llvm_name() << "(";
    for (size_t i = 0; i < params.size(); ++i) {
        if (i) os << ", ";
            os << params[i]->type->llvm_name() << " " << params[i]->val;
    }
    os << ")\n";
    return ref;
}

} // namespace ast

} // namespace compiler
