#include "ast.hpp"

#include <iostream>

#include "namespace.hpp"
#include "builtin.hpp"
#include "types.hpp"

namespace compiler {

namespace ast {

Program::Program(Functions *funcs, Dcls *decls, Stmts *stmts)
    : funcs_(funcs),
      main_(builtin::IntTypeId(),
            identifiers::GetId("main"),
            std::vector<std::pair<Id*, Id*>>(),
            decls,
            stmts) {  }

void Program::print(int lvl) {
    std::cout << "Program\n";
    std::cout << "\tfunctions:\n";
    for (auto func : *funcs_) {
        func->print(lvl+2);
    }
    std::cout << "\tmain:\n";
    main_.print(lvl+2);
    std::cout << '\n';
}

void Program::llvm_output(std::ostream& os, int local_var_count) {
    os << "; ModuleID = '<stdin>'\n";
    os << "source_filename = \"TODO\"\n";
    os << "target datalayout = \"e-m:e-i64:64-f80:128-n8:16:32:64-S128\"\n";
    os << "target triple = \"x86_64-pc-linux-gnu\"\n";
    os << "\n";
    os << "@.io.int = private unnamed_addr constant [3 x i8] c\"%d\\00\", align 1\n";
    os << "\n\n";

    for (Function* func : *funcs_) {
        func->llvm_put(os);
    }
    main_.llvm_put(os);
    
    os << "declare i32 @__isoc99_scanf(i8*, ...)\n";
    os << "declare i32 @printf(i8*, ...)\n";
}

} //namespace ast

} // namespace compiler

