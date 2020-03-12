%{
#include "calculator.tab.h"
%}

%option yylineno

%%

[ \t\r\n]+              ;
"="                     return EQUAL;
"+"                     return PLUS;
"-"                     return MINUS;
"*"                     return MULT;
"/"                     return DIV;
"("                     return LPAR;
")"                     return RPAR;
";"                     return SEMICOLON;
[0-9]+                  { yylval.num = atoi(yytext); return INT; }
r[0-9]                  { yylval.str = strdup(yytext); return REGISTER; }

[^ \t\r\n+\-*/;()r=0-9] printf("%d: ERROR: Unrecognized symbol %s\n", yylineno, yytext);


