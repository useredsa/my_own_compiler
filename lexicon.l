%{
#include "lexicon.h"
int check_id_size();
void unfinished_string();
int check_integer();
void lexical_error();
int literalSize = 0;
%}

%option yylineno
digit               [0-9]
letter              [a-zA-Z]
integer             {digit}+
%x STRING_COND
%x COMMENT_COND

%%

[ \n\t]+                            ;
{integer}                           {
    									long long val = atoll(yytext);
    									if (val >= (1LL<<31) || val < -(1LL<<31)) {
        									printf("%d: WARNING: Integer constant out of range\n", yylineno);
    									}
    									return INT;
									}
 /* \"([^"\n]|\\\")*\"                  return STRING; */
 /* \"([^"\n]|\\\")*                    printf("%d: ERROR: Detected an unfinished string literal\n", yylineno); */
 /* Comments */
"(*"								BEGIN(COMMENT_COND);
<COMMENT_COND>.                     ;
<COMMENT_COND>"*)"                  BEGIN(INITIAL);
 /* Strings */
\"                                  BEGIN(STRING_COND), literalSize = 0, yymore(); 
<STRING_COND>([^"]|\\\")            {
										if (yyleng + 2 >= 1000) { //TODO change
    										printf("%d: ERROR: String literal surpasses maximum size\n")
										}
                                        yymore();
                                    }
<STRING_COND>\"                     return STRING;
 /* Keywords */
program                             return PROGRAM;
function                            return FUNCTION;
const                               return CONST;
var                                 return VAR;
integer                             return INTEGER;
begin                               return BEGINN;
end                                 return END;
if                                  return IF;
then                                return THEN;
else                                return ELSE;
while                               return WHILE;
do                                  return DO;
for                                 return FOR;
to                                  return TO;
write                               return WRITE;
read                                return READ;
 /* Operators */
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
({letter}|_)({letter}|{digit}|_){0,15}    return ID;
({letter}|_)({letter}|{digit}|_){16,16}   return check_id_size();
.                                   printf("\nERROR, unrecognized symbol %s\n", yytext);
%%

int check_id_size() {
  if (yyleng > MAX_ID_LENGTH) {
    printf("\nERROR, oversized identifier %s", yytext);
    return 0;
  } else {
    return ID;
  }
}

// void unfinished_string() {
//   printf("\nERROR, unfinished string in line %d", yylineno);
// }

// int check_integer() {
//   // ¿Cómo reconocer enteros de más de 64 bits?
//   long long ll = atol(yytext);
//   if (abs(ll) > (1<<31)) {
//     printf("\nERROR, oversized integer %lld", ll);
//     return 0;
//   } else {
//     return INTLITERAL;
//   }
// }

// void lexical_error() {
//   printf("\nERROR, unrecognized symbol %s\n", yytext);
// }

void main() {
  int i;
  while (i=yylex())
    printf("%d %s\n", i, yytext);
  printf("END OF LEXICAL ANALYSIS\n");
}

