#include <stdio.h>
#include <stdlib.h>
#include "ast.hpp" //TODO remove, should be included below, but it isn't
#include "syntax.tab.hh"

// extern File* yyin
extern int yyparse();
extern T_program ast_root;

int main(int argc, char* argv[]) {
    if (yyparse()) {
        printf("Syntactic error\n");
    } else {
        printf("Everthing OK!\n");
        ast_root.print();
    }
}

