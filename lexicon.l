%{
#include "lexicon.h"
int check_id_size();
int numErrors = 0;
int literalSize = 0;
const int MAX_STRING_LITERAL_SIZE = 1<<7; // 7Kb
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
                                            printf("%d: WARNING: Integer literal out of range\n", yylineno);
                                            numErrors++;
                                        }
                                        return INT;
                                    }
 /* \"([^"\n]|\\\")*\"                  return STRING; */
 /* \"([^"\n]|\\\")*                    printf("%d: ERROR: Detected an unfinished string literal\n", yylineno); */

 /* Comments */
                                    //TODO One-line comments
"(*"                                BEGIN(COMMENT_COND);
<COMMENT_COND>.                     ;
                                    //TODO Detect and ignore line breaks
                                    //TODO Detect non-closed comment with EOF
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
                                    //TODO Detect and treat line breaks
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
({letter}|_)({letter}|{digit}|_){16,16}   return check_id_size(); //TODO Check id size here
.                                   {
                                        printf("\nERROR, unrecognized symbol %s\n", yytext);
                                        numErrors++;
                                    } //TODO Detect unrecognized symbol sequences
%%

int check_id_size() { //TODO Continue reading the identifier until flex buffer overflows
  if (yyleng > MAX_ID_LENGTH) { 
    printf("\nERROR, oversized identifier %s", yytext);
    return 0;
  } else { //TODO We never enter here
    return ID;
  }
}

void main() {
  int i;
  while (i=yylex())
    //TODO Print things with the format:
    //  Token: *name_of_token* ; [lexeme: *lexeme*]
    // print lexeme when name_of_token is ID, INT or STRING
    //TODO Print errors with the format:
    //  ERROR: *error_description* in line *line*[: *lexeme*]
    // print lexeme when error_description is related to ID, INT or unrecognized symbol
    printf("%d %s\n", i, yytext);
  printf("END OF LEXICAL ANALYSIS\n");
  if (numErrors > 0) {
      return -1;
  }
  return 0;
}

