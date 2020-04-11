%{
#include <stdarg.h>
#include <string>
#include <vector>
using namespace std; //TODO remove

#include "ast.hpp"
#include "syntax.tab.hpp"

int check_id_size();
int numErrors = 0;
int numWarnings = 0;
int lineStart = 0;
string* strlit;
const int MAX_STRING_LITERAL_SIZE = 1<<7; // 7Kb

void logerr(const string& fmt, ...);
void logwar(const string& fmt, ...);

%}

%option noyywrap
%option yylineno
digit               [0-9]
letter              [a-zA-Z]
integer             {digit}+
unrecognized        [^0-9a-zA-Z()".,:;=+\-*/\\ \t\r\n]
%x STRING_COND
%x STRING_ESCAPE_COND
%x COMMENT_COND
%x INLINE_COMM_COND
%x LARGE_ID_COND

%%

[ \t\r\n]+                          ;
{integer}                           {
                                      long long val = atoll(yytext);
                                      if (val >= (1LL<<31) || val < -(1LL<<31))
                                        logwar("Integer literal out of range");
                                      yylval.intlit = val;
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
"\""                                {
                                      strlit = new string();
                                      yymore();  //TODO Esto supongo que ya no tiene sentido
                                      BEGIN(STRING_COND);
                                    }
<STRING_COND>"\n"                   {
                                      delete strlit;
                                      logerr("Unclosed String");
                                      yyless(yyleng-1);  //TODO Esto supongo que sobra tambié
                                      BEGIN(INITIAL);
                                      return STRING;
                                    }
<STRING_COND>[^\"\n]                {
                                      //TODO No recuerdo por qué era el "+ 2", así que he
                                      //     dejado una comparación equivalente a la anterior
                                      if (strlit->length() + yyleng + 2 >= MAX_STRING_LITERAL_SIZE) {
                                        logerr("String literal surpasses maximum size");
                                        exit(-1);  //TODO Seguir parseando?
                                      }
                                      if (yytext[yyleng-1] == '\\') {
                                        strlit->append(yytext, yyleng-1);
                                        BEGIN(STRING_ESCAPE_COND);
                                      } else {
                                        yymore();
                                      }
                                    }
<STRING_ESCAPE_COND>"\\"            strlit->push_back('\\'), BEGIN(STRING_COND);
<STRING_ESCAPE_COND>"\""            strlit->push_back('\"'), BEGIN(STRING_COND);
<STRING_ESCAPE_COND>"t"             strlit->push_back('\t'), BEGIN(STRING_COND);
<STRING_ESCAPE_COND>"r"             strlit->push_back('\r'), BEGIN(STRING_COND);
<STRING_ESCAPE_COND>"n"             strlit->push_back('\n'), BEGIN(STRING_COND);
 /*TODO He probado printf("\x") en C con varias letras y suele salir x*/
<STRING_ESCAPE_COND>.               logerr("Unknown escape sequence"), BEGIN(STRING_COND);
<STRING_COND>"\""                   {
                                      strlit->append(yytext, yyleng-1);
                                      yylval.strlit = strlit;
                                      BEGIN(INITIAL);
                                      return STRING;
                                    }

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

 /* Identifiers */
({letter}|_)({letter}|{digit}|_){0,15}    {
                                            string lexem = string(yytext, yyleng);
                                            int& id = AST::id_lookup[lexem];
                                            if (id == 0) {
                                                id = AST::id_register(lexem);
                                            }
                                            yylval.raw_id = id;
                                            return ID;
                                          }

({letter}|_)({letter}|{digit}|_){16,16}   {
                                            yyless(16);
                                            logerr("Oversized identifier (using: %16s)", yytext);
                                            BEGIN(LARGE_ID_COND);
                                            return ID;
                                          }
<LARGE_ID_COND>({letter}|{digit}|_)       ;
<LARGE_ID_COND>.                          { yyless(0); BEGIN(INITIAL); }

 /*TODO Con ayuda de una condición, podemos protegernos de un overflow*/
{unrecognized}+                           logerr("Unrecognized symbols %s", yytext);


%%

void logerr(const string& fmt, ...) {
    // numErrors++;
    // va_list args;
    // va_start(args, fmt);

    // fprintf(stderr, "\n%d: ERROR: ", yylineno);
    // vfprintf(stderr, fmt, args);
    // fprintf(stderr, "\n\n");
    // va_end(args);
}

void logwar(const string& fmt, ...) {
    // numWarnings++;
    // va_list args;
    // va_start(args, fmt);

    // fprintf(stderr, "\n%d: WARNING: ", yylineno);
    // vfprintf(stderr, fmt, args);
    // fprintf(stderr, "\n\n");
    // va_end(args);
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

