%{ /* Pre declarations */
#include <iostream>
#include <string>
#include "ast.hpp"
using namespace std; //TODO remove

extern int yylex();
void yyerror(const char* msg);
extern int yylineno;

T_program ast_root;

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

%type <intlit>     "int_lit"
%type <strlit>     "str_lit"
%type <func>       function
%type <funcs>      functions
%type <decls>      declarations
%type <sttm>       statement
%type <sttms>      statements
%type <consts>     constants
%type <comp_sttm>  compound_statement
%type <sttms>      optional_statements
%type <exp>        expression
%type <exps>       expressions
%type <assig>      assignment
%type <print_item> print_item
%type <print_list> print_list
%type <args>       arguments
%type <raw_id>     "id"
%type <ids>        identifiers 
%type <type>       type

%union {
    int                   intlit;
    std::string*          strlit;
    t_function*           func;
    t_functions*          funcs;
    t_declarations*       decls;
    t_statement*          sttm;
    t_statements*         sttms;
    t_compound_statement* comp_sttm;
    t_constants*          consts;
    t_expression*         exp;
    t_expressions*        exps;
    t_assignment*         assig;
    t_print_item*         print_item;
    t_print_list*         print_list;
    t_arguments*          args;
    int                   raw_id;
    t_identifiers*        ids;
    t_type*               type;
}

%% /* Production Rules */

program:
    "program" "id" "(" ")" ";" functions declarations compound_statement "." {
        ast_root = T_program($6, $7, $8);
        cout << "program!\n";
    }
    ;

functions:
    functions function ";" {
        $$ = $1;
        $$->push_back($2);
    }
    |
    {
        $$ = new t_functions();
    }
    ;

function: 
    "function" "id" "(" "const" identifiers ":" type ")" ":" type
    declarations
    compound_statement {
        $$ = new t_function(new t_id($2), $5, $7, $10, $11, $12);
    }
    ;

declarations:
    declarations "var" identifiers ":" type ";" {
        $$ = $1;
        $$->add_identifiers($3, $5);
        // free($3);
    }
    |
    declarations "const" constants ";" {
        $$ = $1;
        $$->add_constants($3);
        // free($3);
    }
    |
    {
        $$ = new t_declarations();
    }
    ;

compound_statement:
    "begin" optional_statements "end" {
        $$ = new t_compound_statement($2);
    }
    ;

optional_statements:
    statements {
        $$ = $1;
    }
    |
    {
        $$ = NULL;  //TODO
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
    assignment {
        $$ = $1;
    }
    |
    "if" expression "then" statement {
        $$ = new t_if_then_statement($2, $4);
    }
    |
    "if" expression "then" statement
    "else" statement {
        $$ = new t_if_then_else_statement($2, $4, $6);
    }
    |
    "while" expression "do" statement {
        $$ = new t_while_statement($2, $4);
    }
    |
    "for" "id" ":=" expression "to"
    expression "do" statement {
        $$ = new t_for_statement(new t_id($2), $4, $6, $8);
    }
    |
    "write" "(" print_list ")" {
        for (auto print_item : *($3)) {
            cout << print_item->str_val() << ' ';
        }
        cout << endl;
        $$ = &auxiliar;
    }
    |
    "read"  "(" read_list  ")" {
        $$ = &auxiliar;
    }
    |
    compound_statement {
        $$ = $1;
    }
    ;

assignment:
    "id" ":=" expression {
        $$ = new t_assignment(new t_id($1), $3);
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
    }
    |
    expression "-" expression {
        $$ = new t_binary_op(t_binary_op::valid_op::minus, $1, $3);
    }
    |
    expression "*" expression {
        $$ = new t_binary_op(t_binary_op::valid_op::asterisk, $1, $3);
    }
    |
    expression "/" expression {
        $$ = new t_binary_op(t_binary_op::valid_op::slash, $1, $3);
    }
    |
    "-" expression %prec UMINUS {
        $$ = new t_unary_op(t_unary_op::valid_op::minus, $2);
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
        //TODO Comprobar que la función existe
    }
    ;

arguments:
    expressions {
        $$ = $1;
    }
    |
    {
        //TODO
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
        $$ = &t_builtin_types::integer;
    }
    ;

constants:
    constants "," assignment {
        $$->push_back($3); //TODO asignación?
    }
    |
    assignment {
        $$ = new t_constants();
        $$->push_back($1);
    }
    ;

print_list:
    print_list "," print_item {
        $$->push_back($3);
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
