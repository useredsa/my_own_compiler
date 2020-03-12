%{ /* Pre declarations */
#include<stdio.h>
extern int yylex();
void yyerror(const char* msg);
extern int yylineno;
int reg[10];
%} /* Grammar tokens */

// Data Types
%token INTEGER   "integer"
%token STRING    "string"
// Keywords
%token PROGRAM   "program"
%token FUNCTION  "function"
%token CONST     "const"
%token VAR       "var"
%token INTLIT    "intlit"
%token BEGINN    "begin"
%token END       "end"
%token IF        "if"
%token THEN      "then"
%token ELSE      "else"
%token WHILE     "while"
%token DO        "do"
%token FOR       "for"
%token TO        "to"
%token WRITE     "write"
%token READ      "read"
// Operators
%token ID        "id"
%token SEMICOL   ";"
%token COLON     ":"
%token DOT       "."
%token COMMA     ","
%token PLUSOP    "+"
%token MINUSOP   "-"
%token MULTOP    "*"
%token DIVOP     "/"
%token LBRACKET  "("
%token RBRACKET  ")"
%token ASSIGNOP  ":="
// %token <str> REGISTER  "register"
// %token <num> INT       "integer"

%start program
%define parse.error verbose

// %type <num> expression term factor

%union {
    int num;
    char* str;
}

%% /* Production Rules */

program        : "program" "id" "(" ")" ";"
                 functions
                 declarations
                 compound_statement "."             { printf("alberto!"); }
               ;

functions      : functions function ";"             {  }
               |                                    {  }
               ;
function       : "function" "id"  "("
                 "const" "identifiers" ":" "type"
                 ")" ":" "type"
                 declarations
                 compound_statement                 {  }
               ;

declarations   : declarations
                 "var" identifiers ":" type ";"
               | declarations
                 "const" constants ";"              {  }
               |                                    {  }
               ;

compound_statement  : "begin" optional_statements "end"  {  }
                    ;
optional_statements : statements                         {  }
                    |                                    {  }
                    ;
statements          : statements ";" statement           {  }
                    | statement                          {  }
                    ;
statement           : "id" ":=" expression               {  }
                    | "if" expression "then" statement   {  }
                    | "if" expression "then" statement
                      "else" statement                   {  }
                    | "while" expression "do" statement  {  }
                    | "for" "id" ":=" expression "to"
                      expression "do" statement          {  }
                    | "write" "(" print_list ")"         {  }
                    | "read"  "(" read_list  ")"         {  }
                    | compound_statement                 {  }
                    ;

expressions    : expressions "," expression         {  }
               | expression                         {  }
               ;
expression     : expression "+" expression          {  }
               | expression "-" expression          {  }
               | expression "*" expression          {  }
               | expression "/" expression          {  }
               | "-" expression                     {  }
               | "(" expression ")"                 {  }
               | "id"                               {  }
               | "intlit"                           {  }
               | "id" "(" arguments ")"             {  }
               ;

arguments      : expressions                        {  }
               |                                    {  }
               ;

identifiers    : identifiers "," "id"               {  }
               | "id"                               {  }
               ;

type           : "integer"                          {  }
               ;
constants      : constants "," "id" ":=" expression {  }
               | "id" ":=" expression               {  }
               ;

print_list     : print_list "," print_item          {  }
               | print_item
               ;
print_item     : expression                         {  }
               | "string"
               ;

read_list      : read_list "," read_item            {  }
               | read_item                          {  }
               ;
read_item      : "id"                               {  }
               ;

%%

void yyerror(const char* msg) {
    printf("%d: ERROR: %s\n", yylineno, msg);
}
