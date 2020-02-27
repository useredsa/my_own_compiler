%{
#include "lexicon.h"
int check_id_size();
int numErrors = 0;
int literalSize = 0;
const int MAX_STRING_LITERAL_SIZE = 1<<7; // 7Kb
                                    //TODO One-line comments
                                    //TODO Detect and ignore line breaks
                                    //TODO Detect non-closed comment with EOF
                                    //TODO Detect and treat line breaks
                                    //TODO Detect unrecognized symbol sequences
%}

%option yylineno
digit               [0-9]
letter              [a-zA-Z]
integer             {digit}+
%x STRING_COND
%x COMMENT_COND
%x LARGE_ID_COND

%%

[ \n\t]+                            ;
{integer}                           {
                                        long long val = atoll(yytext);
                                        if (val >= (1LL<<31) || val < -(1LL<<31)) {
                                            printf("%d: WARNING: Integer literal out of range\n", yylineno);
                                            numErrors++;
                                        }
                                        return INT;
                                    }
 /* \"([^"\n]|\\\")*\"                  return STRING; */
 /* \"([^"\n]|\\\")*                    printf("%d: ERROR: Detected an unfinished string literal\n", yylineno); */

 /* Comments */
"(*"                                BEGIN(COMMENT_COND);
<COMMENT_COND>.                     ;
<COMMENT_COND>"*)"                  BEGIN(INITIAL);

 /* Strings */
\"                                  BEGIN(STRING_COND), literalSize = 0, yymore(); 
<STRING_COND>([^\"\n]|\\\")         {
                                      if (yyleng + 2 >= MAX_STRING_LITERAL_SIZE) {
                                        printf("%d: ERROR: String literal surpasses maximum size\n", yylineno);
                                        exit(-1);
                                      }
                                      yymore();
                                    }
<STRING_COND>\"                     { BEGIN(INITIAL); return STRING; }

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
({letter}|_)({letter}|{digit}|_){16,16}   yymore(), BEGIN(LARGE_ID_COND);
<LARGE_ID_COND>({letter}|{digit}|_)       ;
<LARGE_ID_COND>.                          {
                                            yyless(17);
                                            printf("%d: ERROR: oversized identifier (using: %s)\n", yylineno, yytext);
                                            BEGIN(INITIAL);
                                            return ID;
                                          }
.                                   {
                                      printf("\nERROR, unrecognized symbol %s\n", yytext);
                                      numErrors++;
                                    } 
%%

int main() {
  int i;
  while (i=yylex()) {
    //TODO Print errors with the format:
    //  ERROR: *error_description* in line *line*[: *lexeme*]
    // print lexeme when error_description is related to ID, INT or unrecognized symbol
    printf("Lexeme: %s\t; [token: %s]\n", yytext, int_to_lexeme(i));
  }
  printf("END OF LEXICAL ANALYSIS\n");
  if (numErrors > 0) {
    fprintf(stderr, "Some errors were found\n");
    return -1;
  }
  return 0;
}

