%{ /* Pre declarations */
#include<stdio.h>
extern int yylex();
void yyerror(const char* msg);
extern int yylineno;
%} /* Grammar tokens */

%token PLUS      "+"
%token MINUS     "-"
%token MULT      "*"
%token DIV       "/"
%token LPAR      "("
%token RPAR      ")"
%token SEMICOLON ";"
%token INT       "integer"

%start sentence_block
%define parse.error verbose

%% /* Production Rules */

sentence_block : expression ";" sentence_block  { printf("RESULT: %d\n", $1); }
               | ";" sentence_block             { printf("EMPTY\n"); }
               | expression ";"                 { printf("RESULT: %d\n", $1); }
               | expression                     { printf("RESULT: %d\n", $1); }
               |
               ;

expression : expression "+" term                { $$ = $1+$3; }
           | expression "-" term                { $$ = $1-$3; }
           | term
           ;

term   : term "*" factor                    { $$ = $1*$3; }
       | term "/" factor                    { $$ = $1/$3; }
       | factor
       ;

factor : "integer"
       | "(" expression ")"                 { $$ = $2; }
       ;


%%

void yyerror(const char* msg) {
    printf("%d: ERROR: %s\n", yylineno, msg);
}
