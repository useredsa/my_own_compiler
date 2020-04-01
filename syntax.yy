%{ /* Pre declarations */
#include<iostream>
#include "ast.hpp"
using namespace std; //TODO remove

extern int yylex();
void yyerror(const char* msg);
extern int yylineno;
int reg[10];
%} /* Grammar tokens */

// Data Types
%token INTEGER   "integer"
// Literals
%token INTLIT    "int_lit"
%token STRING    "str_lit"
// Keywords
%token PROGRAM   "program"
%token FUNCTION  "function"
%token CONST     "const"
%token VAR       "var"
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

// Operator precedence
%left  "+" "-"
%left  "*" "/"
%right UMINUS

%start program
%define parse.error verbose

%type <intlit> "int_lit"
%type <strlit> "str_lit"
%type <func> function
%type <funcs> functions
%type <sttm> statement
%type <sttms> statements
%type <sttms> compound_statement
%type <sttms> optional_statements
%type <exp>     expression
%type <exps>    expressions
%type <print_item> print_item
%type <print_list> print_list
%type <args> arguments
%type <raw_id> "id"
%type <ids> identifiers 

%union {
    int intlit;
    string* strlit;
    t_function* func;
    t_functions* funcs;
    t_statement* sttm;
    t_statements* sttms;
    t_expression* exp;
    t_expressions* exps;
    t_print_item* print_item;
    t_print_list* print_list;
    t_arguments* args;
    int raw_id;
    t_id* id;
    t_identifiers* ids;
}

%% /* Production Rules */

program:
    "program" "id" "(" ")" ";" functions
    declarations compound_statement "." {
        printf("program!\n");
    }
    ;

functions:
    functions function ";" {
        $$->push_back($2);
    }
    |
    {
        $$ = new t_functions();
    }
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

compound_statement:
    "begin" optional_statements "end" {
        $$ = $2;
    }
    |
    {
        $$ = new t_statements();
    }
    ;

optional_statements:
    statements {
        $$ = $1;
    }
    |
    {
        $$ = new t_statements();
    }
    ;
statements:
    statements ";" statement {
        $$ = $1;
        $$->push_back($3);
    }
    |
    statement {
        $$ = new t_statements();
        $$->push_back($1);
    }
    ;

statement:
    "id" ":=" expression {
        
    }
    |
    "if" expression "then" statement {
        
    }
    |
    "if" expression "then" statement
    "else" statement {
        
    }
    |
    "while" expression "do" statement {
        
    }
    |
    "for" "id" ":=" expression "to"
    expression "do" statement {
        
    }
    |
    "write" "(" print_list ")" {
        for (auto print_item : *($3)) {
            cout << print_item->str_val() << ' ';
        }
        cout << endl;
    }
    |
    "read"  "(" read_list  ")" {
        
    }
    |
    compound_statement {
        
    }
    ;

expressions:
    expressions "," expression {
        $$ = $1;
        $$->push_back($3);
    }
    |
    expression {
        $$ = new t_expressions($1);
    }
    ;

expression:
    expression "+" expression {
        $$ = new t_binary_op(t_binary_op::valid_op::plus, $1, $3);
        //TODO cout << $1->val() << " + " << $3->val() << " = " << $$->val() << endl;
    }
    |
    expression "-" expression {
        $$ = new t_binary_op(t_binary_op::valid_op::minus, $1, $3);
        //TODO cout << $1->val() << " - " << $3->val() << " = " << $$->val() << endl;
    }
    |
    expression "*" expression {
        $$ = new t_binary_op(t_binary_op::valid_op::asterisk, $1, $3);
        //TODO cout << $1->val() << " * " << $3->val() << " = " << $$->val() << endl;
    }
    |
    expression "/" expression {
        $$ = new t_binary_op(t_binary_op::valid_op::slash, $1, $3);
        //TODO cout << $1->val() << " / " << $3->val() << " = " << $$->val() << endl;
    }
    |
    "-" expression %prec UMINUS {
        $$ = new t_unary_op(t_unary_op::valid_op::minus, $2);
        //TODO cout << " - " << $2->val() << " = " << $$->val() << endl;
    }
    |
    "(" expression ")" {
        $$ = $2;
    }
    | "id" {
        $$ = new t_id($1);
    }
    |
    "int_lit" {
        $$ = new t_int_lit($1);
    }
    |
    "id" "(" arguments ")" {
        
    }
    ;

arguments:
    expressions {
        $$ = $1;
    }
    |
    {
        $$ = new t_arguments();
    }
    ;

identifiers:
    identifiers "," "id" {
        $$->push_back(new t_id($3));
    }
    |
    "id" {
        $$ = new t_identifiers();
        $$->push_back(new t_id($1));
    }
    ;

type:
    "integer" {

    }
    ;

constants:
    constants "," "id" ":=" expression {
        
    }
    |
    "id" ":=" expression {

    }
    ;

print_list:
    print_list "," print_item {
        $1->push_back($3);
    }
    |
    print_item {
        $$ = new t_print_list();
        $$->push_back($1);
    }
    ;

print_item:
    expression {
        $$ = $1;
    }
    |
    "str_lit" {
        $$ = new t_str_lit($1);
    }
    ;

//TODO better do after llvm translation (read is not necessary to test everything)
read_list      : read_list "," read_item            {  }
               | read_item                          {  }
               ;

read_item      : "id"                               {  }
               ;

%%

void yyerror(const char* msg) {
    printf("%d: ERROR: %s\n", yylineno, msg);
}
