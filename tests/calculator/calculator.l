%{
#include "calculator.tab.h"
%}

%option yylineno

%%

[ \t\r\n]+            ;
"+"                   return PLUS;
"-"                   return MINUS;
"*"                   return MULT;
"/"                   return DIV;
"("                   return LPAR;
")"                   return RPAR;
";"                   return SEMICOLON;
[0-9]+                { yylval = atoi(yytext); return INT; }
[^ \t\r\n+\-*/;()0-9] printf("%d: ERROR: Unrecognized symbol %s\n", yylineno, yytext);


