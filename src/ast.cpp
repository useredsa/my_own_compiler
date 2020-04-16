#include "ast.hpp"

#include <iostream>

#include "builtin.hpp"

namespace AST {

t_program::t_program(t_functions *funcs, t_declarations *decls,
                     t_statements *stmts) :
                     functions_(funcs),
                     main_(t_id::named("integer"),
                           t_id::named("main"),
                           std::vector<std::pair<t_id*, t_id*>>(),
                           decls,
                           stmts) {  }

void t_program::print(int lvl) {
    std::cout << "Program\n";
    std::cout << "\tfunctions:\n";
    for (auto func : *functions_) {
        func->print(lvl+2);
    }
    std::cout << "\tmain:\n";
    main_.print(lvl+2);
    std::cout << '\n';
}

void t_program::llvm_output(std::ostream& os, int local_var_count) {
    builtin::llvm_register_builtins();
    
    os << "; ModuleID = '<stdin>'\n";
    os << "source_filename = \"TODO\"\n";
    os << "target datalayout = \"e-m:e-i64:64-f80:128-n8:16:32:64-S128\"\n";
    os << "target triple = \"x86_64-pc-linux-gnu\"\n";
    os << "\n";
    os << "@.io.int = private unnamed_addr constant [3 x i8] c\"%d\\00\", align 1\n";
    os << "\n\n";

    main_.llvm_put(os);
    
    os << "declare i32 @__isoc99_scanf(i8*, ...)\n";
    os << "declare i32 @printf(i8*, ...)\n";
}

}  //namespace AST
