%{ /* Pre declarations */
#include <iostream>
#include <string>
#include "ast.hpp"
#include "builtin.hpp"
#include "namespace.hpp"
using namespace std; //TODO remove
using namespace AST;

extern int yylex();
void yyerror(const char* msg);
extern int yylineno;

t_program* ast_root;

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
%token NAME      "name"
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

%type <intlit>   "int_lit"
%type <strlit>   "str_lit"
%type <func>     function
%type <funcs>    functions
%type <decls>    declarations
%type <sttm>     statement
%type <sttms>    statements
%type <sttms>    compound_statement
%type <consts>   constants
%type <sttms>    optional_statements
%type <exp>      expression
%type <exps>     expressions
%type <assig>    assignment
%type <exp>      print_item
%type <exps>     print_list
%type <id>       read_item
%type <ids>      read_list
%type <exps>     arguments
%type <name>     "name"
%type <id>       dcl_id
%type <id>       undcl_id
%type <ids>      undcl_ids
%type <unr_name> unresolved_name
%type <id>       type

%code requires {
    #include <iostream>
    #include <string>
    #include "ast.hpp"
    #include "namespace.hpp"
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
    t_unresolved_name*          unr_name;
    std::vector<t_expression*>* args;
}

//TODO Destructors of discarded symbols
// %destructor { } <intlit>  //TODO Include types with no destructor defined
// %destructor { delete $$; } <*>  // Default destructor for type defined symbols

%% /* Production Rules */

program:
    "program" "name" "(" ")" ";" functions declarations compound_statement "." {
        ast_root = new t_program($6, $7, $8);
        // std::cout << "program!\n";
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
    {
        t_namespace* namesp = t_namespace::create();
        t_namespace::switch_namespace(namesp);
    }
    "function" undcl_id "(" "const" undcl_ids ":" type ")" ":" type
    declarations compound_statement {
        std::vector<std::pair<t_id*, t_id*>> args;
        args.reserve($6->size());
        for (t_id* id : *$6) {
            args.emplace_back(id, $8);
        }
        $$ = new t_function($3, $11, args, $12, $13);
        t_namespace::switch_to_parent_namespace();
        //TODO free $6
    }
    |
    error declarations compound_statement {
        
    }
    ;

declarations:
    declarations "var" undcl_ids ":" type ";" {
        $$ = $1;
        $$->add_identifiers(*$3, $5);
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
    "for" undcl_id ":=" expression "to"
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
    dcl_id ":=" expression {
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
    | dcl_id {
        $$ = $1;
    }
    |
    "int_lit" {
        $$ = new t_int_lit($1);
    }
    |
    unresolved_name "(" arguments ")" {
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
        $$ = new t_expressions();
    }
    ;

type:
    "integer" {
        $$ = t_namespace::get_id("integer");
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
        $$ = new std::vector<t_id*>();
        $$->push_back($1);
    }
    ;

read_item: //TODO considerar quitar ya que está name
    dcl_id {
        $$ = $1;
    }
    ;

// Additions to the grammar
undcl_id:
    "name" {
        $$ = t_namespace::new_id(*$1);
        //TOOD free?
    }
    ;

dcl_id:
    "name" {
        $$ = t_namespace::get_id(*$1);
    }
    ;

unresolved_name:
    "name" {
        $$ = t_namespace::pending_id(*$1); //TODO hay que revisar su uso
    }
    ;

undcl_ids:
    undcl_ids "," undcl_id {
        $$->push_back($3);
    }
    |
    undcl_id {
        $$ = new std::vector<t_id*>();
        $$->push_back($1);
    }
    ;

%%

void yyerror(const char* msg) {
    printf("%d: ERROR: %s\n", yylineno, msg);
}
