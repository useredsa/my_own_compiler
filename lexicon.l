%{
#include "lexicon.h"
int check_id_size();
int numErrors = 0;
int numWarnings = 0;
int literalSize = 0;
int lineStart = 0;
const int MAX_STRING_LITERAL_SIZE = 1<<7; // 7Kb
%}

%option yylineno
digit               [0-9]
letter              [a-zA-Z]
integer             {digit}+
unrecognized        [^0-9a-zA-Z()".,:;=+\-*/\\ \t\r\n]
%x STRING_COND
%x COMMENT_COND
%x INLINE_COMM_COND
%x LARGE_ID_COND

%%

[ \t\r\n]+                            ;
{integer}                           {
                                        long long val = atoll(yytext);
                                        if (val >= (1LL<<31) || val < -(1LL<<31)) {
                                            printf("%d: WARNING: Integer literal out of range\n", yylineno);
                                            numWarnings++;
                                        }
                                        return INT;
                                    }
 /* \"([^"\n]|\\\")*\"                  return STRING; */
 /* \"([^"\n]|\\\")*                    printf("%d: ERROR: Detected an unfinished string literal\n", yylineno); */

 /* Comments */
"//"                                BEGIN(INLINE_COMM_COND);
<INLINE_COMM_COND>.                 ;
<INLINE_COMM_COND>"\n"              BEGIN(INITIAL);
"(*"                                lineStart = yylineno, BEGIN(COMMENT_COND);
<COMMENT_COND><<EOF>>               {
                                      numErrors++;
                                      printf("%d: ERROR: Unclosed comment starting on line: %d\n", yylineno, lineStart);
                                      BEGIN(INITIAL);
                                    }
<COMMENT_COND>(.|\n)                ;
<COMMENT_COND>"*)"                  BEGIN(INITIAL);

 /* Strings */
\"                                  BEGIN(STRING_COND), literalSize = 0, yymore(); 
<STRING_COND>\n                     {
                                      numErrors++;
                                      printf("%d: ERROR: Unclosed string.", yylineno);
                                      BEGIN(INITIAL);
                                      return STRING;
                                    }
<STRING_COND>([^\"\n]|\\\")         {
                                      if (yyleng + 2 >= MAX_STRING_LITERAL_SIZE) {
                                        numErrors++;
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
({letter}|_)({letter}|{digit}|_){16,16}   {
                                            numErrors++;
                                            yyless(16);
                                            printf("%d: ERROR: oversized identifier (using: %15s)\n", yylineno, yytext);
                                            BEGIN(LARGE_ID_COND);
                                            return ID;
                                          }
<LARGE_ID_COND>({letter}|{digit}|_)       ;
<LARGE_ID_COND>.                          { yyless(1); BEGIN(INITIAL); }
{unrecognized}+                           {
                                            numErrors++;
                                            printf("%d: ERROR: unrecognized symbols %s\n", yylineno, yytext);
                                          } 
%%

int main() {
  int i;
  while (i=yylex()) {
    //TODO Print errors with the format:
    //  ERROR: *error_description* in line *line*[: *lexeme*]
    // print lexeme when error_description is related to ID, INT or unrecognized symbol
    printf("Token: %s\t; [Lexeme: %s]\n", int_to_lexeme(i), yytext);
  }
  printf("END OF LEXICAL ANALYSIS\n");
  if (numErrors || numWarnings) {
    printf("\n--------------------\n\n");
    fprintf(stderr, "COMPILATION PROBLEMS: %d errors %d warnings\n", numErrors, numWarnings);
    return -1;
  }
  return 0;
}

