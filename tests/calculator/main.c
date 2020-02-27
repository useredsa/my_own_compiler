#include <stdio.h>
#include <stdlib.h>

// extern File* yyin
extern int yyparse();

int main(int argc, char* argv[]) {
    if (yyparse()) {
        printf("Syntactic error\n");
    } else {
        printf("Everthing OK!\n");
    }
}
