#include "ast.hpp"

#include <iostream>
#include "builtin.hpp"
#include "types.hpp"

namespace compiler {

namespace ast {

Program::Program(std::vector<Function*>&& funcs, Dcls* decls, std::vector<Stmt>&& stmts)
    : funcs(funcs),
      main(builtin::IntTypeId(),
           identifiers::GetId("main"),
           std::vector<std::pair<Id*, Id*>>(),
           decls,
           std::move(stmts)) {  }

// void Program::llvm_output(std::ostream& os, int local_var_count) {
//     os << "; ModuleID = '<stdin>'\n";
//     os << "source_filename = \"TODO\"\n";
//     os << "target datalayout = \"e-m:e-i64:64-f80:128-n8:16:32:64-S128\"\n";
//     os << "target triple = \"x86_64-pc-linux-gnu\"\n";
//     os << "\n";
//     builtin::LlvmPutStrLits(os);
//     os << "\n\n";

//     for (Function* func : *funcs_) {
//         func->llvm_put(os);
//     }
//     main_.llvm_put(os);
    
//     os << "declare i32 @__isoc99_scanf(i8*, ...)\n";
//     os << "declare i32 @printf(i8*, ...)\n";
//     os << "declare i64 @strlen(i8*)\n";
//     os << "declare i8* @strcpy(i8*, i8*)\n";
// }

} //namespace ast

} // namespace compiler

