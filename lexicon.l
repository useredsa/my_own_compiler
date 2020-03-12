%{
#include <stdarg.h>
#include "syntax.tab.h"
int check_id_size();
int numErrors = 0;
int numWarnings = 0;
int literalSize = 0;
int lineStart = 0;
const int MAX_STRING_LITERAL_SIZE = 1<<7; // 7Kb

void logerr(char* fmt, ...);
void logwar(char* fmt, ...);

%}

%option noyywrap
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

[ \t\r\n]+                          ;
{integer}                           {
                                      long long val = atoll(yytext);
                                      if (val >= (1LL<<31) || val < -(1LL<<31))
                                        logwar("Integer literal out of range");
                                      return INTLIT;
                                    }
 /* \"([^"\n]|\\\")*\"                  return STRING; */
 /* \"([^"\n]|\\\")*                    printf("%d: ERROR: Detected an unfinished string literal\n", yylineno); */

 /* Comments */
"//"                                BEGIN(INLINE_COMM_COND);
<INLINE_COMM_COND>.                 ;
<INLINE_COMM_COND>"\n"              BEGIN(INITIAL);
"(*"                                lineStart = yylineno, BEGIN(COMMENT_COND);
<COMMENT_COND><<EOF>>               {
                                      logerr("Unclosed comment starting on line: %d", lineStart);
                                      BEGIN(INITIAL);
                                    }
<COMMENT_COND>(.|\n)                ;
<COMMENT_COND>"*)"                  BEGIN(INITIAL);

 /* Strings */
\"                                  BEGIN(STRING_COND), literalSize = 0, yymore(); 
<STRING_COND>\n                     {
                                      logerr("Unclosed String");
                                      yyless(yyleng-1);
                                      BEGIN(INITIAL);
                                      return STRING;
                                    }
<STRING_COND>([^\"\n]|\\\")         {
                                      if (yyleng + 2 >= MAX_STRING_LITERAL_SIZE) {
                                        logerr("String literal surpasses maximum size");
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
";"                                 return SEMICOL;
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
                                            yyless(16);
                                            logerr("Oversized identifier (using: %15s)", yytext);
                                            BEGIN(LARGE_ID_COND);
                                            return ID;
                                          }
<LARGE_ID_COND>({letter}|{digit}|_)       ;
<LARGE_ID_COND>.                          { yyless(1); BEGIN(INITIAL); }
{unrecognized}+                           logerr("Unrecognized symbols %s", yytext);


%%

void logerr(char* fmt, ...) {
    numErrors++;
    va_list args;
    va_start(args, fmt);

    fprintf(stderr, "\n%d: ERROR: ", yylineno);
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n\n");
    va_end(args);
}

void logwar(char* fmt, ...) {
    numWarnings++;
    va_list args;
    va_start(args, fmt);

    fprintf(stderr, "\n%d: WARNING: ", yylineno);
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n\n");
    va_end(args);
}

// int main() {
//   int i;
//   while (i=yylex()) {
//     printf("Token: %s\t; Lexeme: `%s`\n", int_to_lexeme(i), yytext);
//   }
//   printf("END OF LEXICAL ANALYSIS\n");
//   if (numErrors || numWarnings) {
//     printf("\n--------------------\n\n");
//     fprintf(stderr, "COMPILATION PROBLEMS: %d errors %d warnings\n", numErrors, numWarnings);
//     return -1;
//   }
//   return 0;
// }

