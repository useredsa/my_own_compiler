%{
#include <stdarg.h>
#include <iostream>
#include <string>
#include <vector>
#include "syntax.tab.hpp"
#include "log.hpp"
using compiler::lexical_log;
using compiler::warning_log;

const int MAX_STRING_LITERAL_SIZE = 1<<7; // 7Kb
int check_id_size();
int lineStart = 0;
std::string* strlit;

%}

%option noyywrap
%option yylineno
%x STRING_COND
%x STRING_ESCAPE_COND
%x COMMENT_COND
%x INLINE_COMM_COND
%x LARGE_ID_COND

digit                           [0-9]
letter                          [a-zA-Z]
integer                         {digit}+
unrecognized                    [^0-9a-zA-Z()".,:;=+\-*/\\ \t\r\n]




%%

[ \t\r\n]+                      ;
 /* Keywords and operators */
program                         return yy::parser::token::PROGRAM;
function                        return yy::parser::token::FUNCTION;
operator                        return yy::parser::token::OPERATOR;
const                           return yy::parser::token::CONST;
var                             return yy::parser::token::VAR;
int                             return yy::parser::token::INT;
str                             return yy::parser::token::STR;
begin                           return yy::parser::token::BEGINN;
end                             return yy::parser::token::END;
if                              return yy::parser::token::IF;
then                            return yy::parser::token::THEN;
else                            return yy::parser::token::ELSE;
while                           return yy::parser::token::WHILE;
do                              return yy::parser::token::DO;
for                             return yy::parser::token::FOR;
to                              return yy::parser::token::TO;
write                           return yy::parser::token::WRITE;
read                            return yy::parser::token::READ;
";"                             return yy::parser::token::SEMICOL;
":"                             return yy::parser::token::COLON;
"."                             return yy::parser::token::DOT;
","                             return yy::parser::token::COMMA;
"+"                             return yy::parser::token::PLUSOP;
"-"                             return yy::parser::token::MINUSOP;
"*"                             return yy::parser::token::MULTOP;
"/"                             return yy::parser::token::DIVOP;
"("                             return yy::parser::token::LBRACKET;
")"                             return yy::parser::token::RBRACKET;
":="                            return yy::parser::token::ASSIGNOP;

 /* Comments */
"//"                            BEGIN(INLINE_COMM_COND);
<INLINE_COMM_COND>.             ;
<INLINE_COMM_COND>"\n"          BEGIN(INITIAL);
"(*"                            lineStart = yylineno, BEGIN(COMMENT_COND);
<COMMENT_COND><<EOF>>           {
                                  lexical_log << "Unclosed comment starting on line: "
                                              << lineStart << "\n";
                                  BEGIN(INITIAL);
                                }
<COMMENT_COND>(.|\n)            ;
<COMMENT_COND>"*)"              BEGIN(INITIAL);


 /* Strings */
"\""                            {
                                  strlit = new std::string();
                                  BEGIN(STRING_COND);
                                }
<STRING_COND>"\n"               {
                                  delete strlit;
                                  lexical_log << "Unclosed String\n";
                                  BEGIN(INITIAL);
                                  return yy::parser::token::STRLIT;
                                }
<STRING_COND>[^\"\n]            {
                                  if (strlit->length() + yyleng + 2 >= MAX_STRING_LITERAL_SIZE) {
                                    lexical_log << "String literal surpasses maximum size\n";
                                    exit(-1);
                                  }
                                  if (yytext[yyleng-1] == '\\') {
                                    strlit->append(yytext, yyleng-1);
                                    BEGIN(STRING_ESCAPE_COND);
                                  } else {
                                    yymore();
                                  }
                                }
<STRING_ESCAPE_COND>"\\"        strlit->push_back('\\'), BEGIN(STRING_COND);
<STRING_ESCAPE_COND>"\""        strlit->push_back('\"'), BEGIN(STRING_COND);
<STRING_ESCAPE_COND>"t"         strlit->push_back('\t'), BEGIN(STRING_COND);
<STRING_ESCAPE_COND>"r"         strlit->push_back('\r'), BEGIN(STRING_COND);
<STRING_ESCAPE_COND>"n"         strlit->push_back('\n'), BEGIN(STRING_COND);
<STRING_ESCAPE_COND>.           {
                                  lexical_log << "Unknown escape sequence\n";
                                  BEGIN(STRING_COND);
                                }
<STRING_COND>"\""               {
                                  strlit->append(yytext, yyleng-1);
                                  BEGIN(INITIAL);
                                  return yy::parser::make_STRLIT(strlit);
                                }


{integer}                       {
                                  long long val = atoll(yytext);
                                  if (val >= (1LL<<31) || val < -(1LL<<31))
                                    warning_log << "Integer literal out of range\n";
                                  return yy::parser::make_INTLIT(val);
                                }


 /* Identifiers */
({letter}|_)({letter}|{digit}|_){0,15}  {
                                  std::string* lexem = new std::string(yytext, yyleng);
                                  return yy::parser::make_NAME(lexem);
                                }

({letter}|_)({letter}|{digit}|_){16,16} {
                                  lexical_log << yylineno << ": Oversized identifier\n";
                                  exit(-1);
                                }

 /*IMPROVEMENT Con ayuda de una condición, podemos protegernos de un overflow*/
{unrecognized}+                 lexical_log << "Unrecognized symbols " << yytext << "\n";


%%

//IMPROVEMENT It would be nice to have a function to debug the parser

