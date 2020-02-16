%{
#include "lexicon.h"
int check_id_size();
void unfinished_string();
int check_integer();
void lexical_error();
%}
%option yylineno
digit               [0-9]
letter              [a-zA-Z]
integer             {digit}+
%%
[ \n\t]+                            ;
{integer}+                          return check_integer();
\"([^"\n]|\\\")*\"		    return STRING;
\"([^"\n]|\\\")* 		    unfinished_string();
program			    return PROGRAM;
function			    return FUNCTION;
const				    return CONST;
var				    return VAR;
integer			    return INTEGER;
begin                               return BEGINN;
end                                 return END;
if				    return IF;
then				    return THEN;
else				    return ELSE;
while 				    return WHILE;
do				    return DO;
for				    return FOR;
to				    return TO;
write                               return WRITE;
read                                return READ;
({letter}|_)({letter}|{digit}|_)*   return check_id_size();
";"                                 return SEMICOLON;
":"                                 return COLON;
"."                                 return DOT;
","                                 return COMMA;
"+"                                 return PLUSOP;
"-"                                 return MINUSOP;
"*"                                 return MULTOP;
"/"                                 return DIVOP;
"("                                 return LBRACKET;		
")"                                 return RBRACKET;
":="                                return ASSIGNOP;
.                                   lexical_error();
%%

int check_id_size() {
  if (yyleng > MAX_ID_LENGTH) {
    printf("\nERROR, oversized identifier %s", yytext);
    return 0;
  } else {
    return ID;
  }
}

void unfinished_string() {
  printf("\nERROR, unfinished string in line %d", yylineno);
}

int check_integer() {
  // ¿Cómo reconocer enteros de más de 64 bits?
  long long ll = atol(yytext);
  if (abs(ll) > (1<<31)) {
    printf("\nERROR, oversized integer %lld", ll);
    return 0;
  } else {
    return INTLITERAL;
  }
}

void lexical_error() {
  printf("\nERROR, unrecognized symbol %s\n", yytext);
}

void main() {
  int i;
  while (i=yylex())
    printf("%d %s\n", i, yytext);
  printf("END OF LEXICAL ANALYSIS\n");
}

