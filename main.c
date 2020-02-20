#include <stdio.h>
#include <stdlib.h>
#include "lexico1.h"

extern char *yytext;
extern int  yyleng;
extern FILE *yyin;
extern int yylex();
FILE *fich;
int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("Uso: %s fichero\n", argv[0]);
        exit(2);
    }
    if ((fich = fopen(argv[1], "r")) == NULL) {
        printf("%s: No se pudo abrir el archivo '%s'", argv[0], argv[1]);
        exit(1);
    }

    int i;
    yyin=fich;
    while (i=yylex()) {
        printf("TOKEN %d",i);
        if(i==ID) printf("LEXEMA %s  LONGITUD %d\n",yytext,yyleng);
        else printf("\n");}
    fclose(fich);
    return 0;
}

