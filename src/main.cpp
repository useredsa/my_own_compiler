#include <stdlib.h>

#include <iostream>
#include <fstream>

#include "errors.hpp"
#include "ast.hpp" //TODO remove, should be included below, but it isn't
#include "builtin.hpp"
#include "syntax.tab.hpp"

using namespace std;

// extern File* yyin
extern int yyparse();
extern Program* ast_root;

int main(int argc, char* argv[]) {
    // cerr << argc << endl;
    builtin::RegisterBuiltins();
    // if (argc != 2) {
    //     cout << "Usage: " << argv[0] << " file.mp\n";
    //     return -1;
    // }
    if (yyparse()) {
        cout << "Syntactic error\n";
    } else {
        cout << "Everthing OK!\n";
        ast_root->print();

        ofstream os("bin/a.llvm");
        ast_root->llvm_output(os);
    }
}

