#include <iostream>
#include "ast.hpp"

namespace AST {

T_program::T_program() {    }

T_program::T_program(t_functions *funcs, t_declarations *decls,
                     t_statements *stmts)
    : functions_(funcs), declarations_(decls), statements_(stmts) {

}

void T_program::print(int lvl = 0) {
    std::cout << "Program\n";
    std::cout << "\tfunctions:\n";
    for (auto funcs : *functions_) {
        funcs->print(lvl+2);
    }
    declarations_->print(lvl+1);
    statements_->print(lvl+1);
    std::cout << '\n';
}

void T_program::llvm_output(std::ostream& os, int local_var_count = 1) {
    os << "; ModuleID = '<stdin>'\n";
    os << "source_filename = \"TODO\"\n";
    os << "target datalayout = \"e-m:e-i64:64-f80:128-n8:16:32:64-S128\"\n";
    os << "target triple = \"x86_64-pc-linux-gnu\"\n";
    os << "\n";
    os << "@.io.int = private unnamed_addr constant [3 x i8] c\"%d\\00\", align 1\n";
    os << "\n\n";

    os << "; Function\n";
    os << "define i32 @main() {\n";
    for (auto st : *statements_) {
        st->llvm_put(os, local_var_count);
    }
    os << "\tret i32 0\n";
    os << "}\n\n";
    
    os << "declare i32 @__isoc99_scanf(i8*, ...)\n";
    os << "declare i32 @printf(i8*, ...)\n";
}

}  //namespace
