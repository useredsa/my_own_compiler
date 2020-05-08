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
    os << "declare i64 @strlen(i8*)\n";
    os << "declare i8* @strcpy(i8*, i8*)\n";
}

void Translator::Output(Fun* fun) {
    //TODO fun->dcls()->llvm_put_constants(os);
    os << "; Fun\n";
    os << "define " << fun->rtype().ty->llvm_name() << " @" << fun->id()->name() << "(";
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
    // ...store in the value for the arguments.
    for (size_t i = 0; i < args.size(); ++i) {
        EasyStore(LocVar(i+1), args[i]);
    }

    // The local variables counter starts at %1 for empty functions,
    // %n+1 for functions with n arguments (due to the previous allocas).
    local_var_count = 1+args.size();
    for (const Stmt& stmt : fun->stmts()) {
        Output(stmt);
    }
    os << "\tret i32 0\n"; //TODO return type!
    os << "}\n\n";
}

// void Translator::AllocaVariables(Dcls* dcls) {
//     for (Id* id : dcls->variables) {
//         EasyAlloca(id->name(), id->var()->type->type()); //TODO este tipo de indirecciones...
//     }
// }

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
    int label_num = local_var_count;  //TODO Llevar cuenta de las etiquetas? 
    ComputedExp cond_exp = Eval(if_stmt->exp);
    os << "\t%" << local_var_count << " = icmp ne i32 " << cond_exp.val << ", 0\n";
    os << "\tbr i1 %" << local_var_count++ << ", label %then" << label_num << ", label %"
       << (std::holds_alternative<EmptyStmt*>(if_stmt->alt_stmt)? "fi" : "else")
       << label_num << "\n";
    os << "then" << label_num << ":\n";
    Output(if_stmt->stmt);
    if (!std::holds_alternative<EmptyStmt*>(if_stmt->alt_stmt)) {
        os << "\tbr label %fi" << label_num << "\n";  
        os << "else" << label_num << ":\n";
        Output(if_stmt->alt_stmt);
        os << "\tbr label %fi" << label_num << "\n";
    }
    os << "fi" << label_num << ":\n";
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
    // control_id = begin  //TODO Lástima no aprovechar el contenido de assignment
    ComputedExp start_exp = Eval(for_stmt->start_exp);
    EasyStore(start_exp.val, for_stmt->rvar.var);
    os << "\tbr label %forcomp" << label_num << "\n";

    Exp aux_id_exp = for_stmt->rvar;
    // Loop until control_id == end
    ComputedExp end_exp = Eval(for_stmt->end_exp);
    os << "forcomp" << label_num << ":\n";
    ComputedExp ctrl_id_exp = Eval(aux_id_exp);
    os << "\t%" << local_var_count << " = icmp ne " << IntTypeId()->type()->llvm_name()
       << " " << ctrl_id_exp.val << ", " << end_exp.val << "\n";
    os << "\tbr i1 %" << local_var_count++ << ", label %forloop" << label_num
       << ", label %afterfor" << label_num << "\n";
    // for body
    os << "forloop" << label_num << ":\n";
    Output(for_stmt->stmt);

    // increment control_id
    os << "\t%" << local_var_count << " = add nsw i32 " << ctrl_id_exp.val << ", 1\n";
    EasyStore(LocVar(local_var_count), for_stmt->rvar.var);
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
            //TODO imprimir error read no soporta cosas que no sean builtin -> a otro sitio
            //DONE
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
            //TODO imprimir error write no soporta cosas que no sean builtin
            //DONE
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
    // if (abstracts_ == kConstant) {
    //     //TODO
    // }
    // //TODO y otro error pero este debe comprobarse antes :/
    // return "ERROR";
    // }
}

template<ast::UnaryOperators op>
ComputedExp Translator::operator()(UnaOp<op>* una_op) {
    //TODO
    return {IntTypeId()->type(), "%-2"};
}

template<ast::BinaryOperators op>
ComputedExp Translator::operator()(BinOp<op>* bin_op) {
    //TODO resolver las funciones en una pasada
    ComputedExp lhs_exp = Eval(bin_op->lhs);
    ComputedExp rhs_exp = Eval(bin_op->rhs);
    // Fun* func = identifiers::GetId("operator" + (char) op)->
    //                      can_be_called({lhs->exp_type(), rhs->exp_type()});
    // assert(func != nullptr);
    // return func->type(); //TODO
    // Fun* func = identifiers::GetId("operator+")->
    //                      can_be_called({lhs->exp_type(), rhs->exp_type()});
    // return func->llvm_put_call(os, local_var_count, {&lhs_str, &rhs_str});
    return {IntTypeId()->type(), "%-2"};
}

ComputedExp Translator::operator()(FunCall* func_call) {
    //TODO las funciones no se pueden resolver con el Id porque eso apunta al mayor namespace
    // que contiene esa función pero puede haber otros debajo con el mismo nombre de función
    // y otros atributos :/. Además hay que hacer la resolución antes.
    // vector<Id*> signature(func_call->args->size());
    // vector<ComputedExp> exps(func_call->args->size());
    // for (size_t i = 0; i < func_call->args->size(); ++i) {
    //     exps[i] = Eval(fun_call->args[i]);
    //     signature[i] = args[i]->exp_type();
    // }
    // Fun* func = id->can_be_called(signature);
    // assert(func != nullptr);
    // if (func == nullptr) {
    //     //TODO error
    // } 

    // string ref = LocVar(local_var_count++);
    // os << "\t" << ref << " = call " << func->type()->type()->llvm_name()
    //    << " @" << func->name() << "(";
    // for (size_t i = 0; i < args->size(); ++i) {
    //     if (i) os << ", ";
    //     os << signature[i]->llvm_type_name() << " " << exp_ress[i];
    // }
    // os << ")\n";
    // return {func->type()->type(), ref};
    return {IntTypeId()->type(), "%-2"};
}

// string Fun::llvm_put_call(std::ostream& os,
//                                       int& local_var_count,
//                                       std::vector<std::string*> params) {
//     return "TODO";
// }

inline ComputedExp Translator::Eval(Exp exp) {
    return std::visit(*this, exp);
}

} // namespace llvm

} // namespace compiler
