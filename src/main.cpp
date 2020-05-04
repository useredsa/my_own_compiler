#include <iostream>
#include <fstream>
#include "syntax.tab.hpp"
#include "builtin.hpp"
#include "ast-printer.hpp"
#include "llvm.hpp"

extern ast::Prog* ast_root;

int main(int argc, char* argv[]) {
    // cerr << argc << endl;
    // if (argc != 2) {
    //     cout << "Usage: " << argv[0] << " file.mp\n";
    //     return -1;
    // }
    // Pre-parsing:
    compiler::builtin::RegisterBuiltins();

    // Parser object
    yy::parser parse{};

    // Parsing
    if (parse()) {
        std::cout << "The program terminated due to syntactic error(s)\n";
    } else {
        compiler::AstPrinter printer(std::cout);
        printer(ast_root);

        // std::ofstream os("bin/a.llvm");
        // compiler::llvm::Translator translator(os);
        // translator.Output(ast_root);
    }
    std::cout << "Everthing OK!\n";
}

