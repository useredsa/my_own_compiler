%{ /* Pre declarations */
#include <iostream>
#include <string>
#include "ast.hpp"
#include "builtin.hpp"
using namespace std; //TODO remove
using namespace AST;

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

// Solution to the "dangling else" problem
%right "then" "else"
// Operator precedence
%left  "+" "-"
%left  "*" "/"
%right UMINUS

%start program
%define parse.error verbose

%type <intlit> "int_lit"
%type <strlit> "str_lit"
%type <func>   function
%type <funcs>  functions
%type <decls>  declarations
%type <sttm>   statement
%type <sttms>  statements
%type <sttms>  compound_statement
%type <consts> constants
%type <sttms>  optional_statements
%type <exp>    expression
%type <exps>   expressions
%type <assig>  assignment
%type <exp>    print_item
%type <exps>   print_list
%type <name>   read_item
%type <names>  read_list
%type <args>   arguments
%type <name>   "id"
%type <id>     name
%type <ids>    names //TODO volver a cambiar a id
%type <id>     type

%code requires {
    #include <string>
    #include "ast.hpp"
    using namespace AST;
}

%union {
    int             intlit;
    std::string*    strlit;
    t_function*     func;
    t_functions*    funcs;
    t_declarations* decls;
    t_statement*    sttm;
    t_statements*   sttms;
    t_constants*    consts;
    t_expression*   exp;
    t_expressions*  exps;
    t_assignment*   assig;
    std::string*    name;
    t_id*           id;
    std::vector<t_id*>*         ids;
    std::vector<t_expression*>* args;
}

%% /* Production Rules */

program:
    "program" "id" "(" ")" ";" functions declarations compound_statement "." {
        ast_root = T_program($6, $7, $8);
        cout << "program!\n";
    }
    |
    error functions declarations compound_statement "." {
        
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
    "function" name "(" "const" names ":" type ")" ":" type
    declarations
    compound_statement {
        $$ = new t_function($2, $5, $7, $10, $11, $12);
    }
    |
    error declarations compound_statement {
        
    }
    ;

declarations:
    declarations "var" names ":" type ";" {
        $$ = $1;
        $$->add_identifiers($3, $5);
        // free($3);
    }
    |
    declarations "const" constants ";" {
        // $$ = $1;
        // $$->add_constants($3);
        // free($3);
    }
    |
    {
        $$ = new t_declarations();
    }
    ;

compound_statement:
    "begin" optional_statements "end" {
        $$ = $2;
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
    "for" name ":=" expression "to"
    expression "do" statement {
        $$ = new t_for_statement($2, $4, $6, $8);
    }
    |
    "write" "(" print_list ")" {
        $$ = new t_write($3);
    }
    |
    "read"  "(" read_list  ")" {
        $$ = new t_read($3);
    }
    |
    compound_statement {
        $$ = $1;
    }
    ;

assignment:
    name ":=" expression {
        $$ = new t_assignment($1, $3);
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
    | name {
        $$ = $1;
    }
    |
    "int_lit" {
        $$ = new t_int_lit($1);
    }
    |
    name "(" arguments ")" {
        //TODO
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

names:
    names "," name {
        $$->push_back($3);
    }
    |
    name {
        $$ = new t_names();
        $$->push_back($1);
    }
    ;

type:
    "integer" {
        $$ = t_id::named("integer");
    }
    ;

constants:
    // constants "," name ":=" expression {
    //     $$ = $1;
    //     $$->push_back($3); //TODO asignación?
    // }
    // |
    // name ":=" expression {
    //     $$ = new t_constants();
    //     $$->push_back($1);
    // }
    ;

print_list:
    print_list "," print_item {
        $$->push_back($3);
    }
    |
    print_item {
        $$ = new t_expressions();
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

read_list:
    read_list "," read_item {
        $$ = $1;
        $$->push_back($3);
    }
    |
    read_item {
        $$ = new t_names();
        $$->push_back($1);
    }
    ;

read_item: //TODO considerar quitar ya que está name
    name {
        $$ = $1;
    }
    ;

// Additions to the grammar
name:
    "id" {
        int& id = name_lookup[*$1];
        if (id == 0) {
            name_register(*$1);
        } else {
            delete $1;
        }
        $$ = new t_name(id);
    }
    ;

%%

void yyerror(const char* msg) {
    printf("%d: ERROR: %s\n", yylineno, msg);
}
