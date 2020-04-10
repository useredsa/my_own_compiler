#ifndef AST_HPP
#define AST_HPP

#include <iostream>
#include "functions.hpp"
#include "statements.hpp"
#include "functions.hpp"
#include "identifiers.hpp"

namespace AST {

class T_program {
  public:
    t_functions *functions;
    t_declarations *declarations;
    t_statements *statements;

    T_program() {  }

    T_program(t_functions *funcs, t_declarations *decls,
              t_statements *stmts)
        : functions(funcs), declarations(decls), statements(stmts) {

    }

    void print(int lvl = 0) {
        std::cout << "Program\n";
        std::cout << "\tfunctions:\n";
        for (auto funcs : *functions) {
            funcs->print(lvl+2);
        }
        declarations->print(lvl+1);
        statements->print(lvl+1);
        std::cout << '\n';
    }

    void llvm_output(std::ostream& os, int local_var_count = 1) {
        os << "; ModuleID = '<stdin>'\n";
        os << "source_filename = \"TODO\"\n";
        os << "target datalayout = \"e-m:e-i64:64-f80:128-n8:16:32:64-S128\"\n";
        os << "target triple = \"x86_64-pc-linux-gnu\"\n";
        os << "\n";
        os << "@.io.int = private unnamed_addr constant [3 x i8] c\"%d\\00\", align 1\n";
        os << "\n\n";

        os << "; Function\n";
        os << "define i32 @main() {\n";
        for (auto st : *statements) {
            st->llvm_put(os, local_var_count);
        }
        os << "\tret i32 0\n";
        os << "}\n\n";
        
        os << "declare i32 @__isoc99_scanf(i8*, ...)\n";
        os << "declare i32 @printf(i8*, ...)\n";
    }
};

}

#endif // AST_HPP

