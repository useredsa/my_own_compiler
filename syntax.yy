%{ /* Pre declarations */
#include <iostream>
#include <string>
#include "ast.hpp"
#include "builtin.hpp"
#include "namespace.hpp"

using namespace compiler::ast;

extern int yylex();
void yyerror(const char* msg);
extern int yylineno;

Program* ast_root;

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

%type <intlit> "int_lit"
%type <strlit> "str_lit"
%type <func>   function
%type <funcs>  functions
%type <decls>  declarations
%type <stmt>   statement
%type <stmts>  statements
%type <stmts>  compound_statement
%type <consts> constants
%type <stmts>  optional_statements
%type <exp>    expression
%type <exps>   expressions
%type <assig>  assignment
%type <exp>    print_item
%type <exps>   print_list
%type <id>     read_item
%type <ids>    read_list
%type <exps>   arguments
%type <name>   "name"
%type <id>     id_dcl
%type <ids>    ids_dcl
%type <id>     id_ref
%type <id>     type

%code requires {
    #include <iostream>
    #include <string>
    #include "ast.hpp"
    #include "namespace.hpp"
    using namespace compiler;
    using namespace compiler::ast;
}

%union {
    int          intlit;
    std::string* strlit;
    Function*    func;
    Functions*   funcs;
    Dcls*        decls;
    IStmt*       stmt;
    Stmts*       stmts;
    Constants*   consts;
    IExp*        exp;
    Exps*        exps;
    Assig*       assig;
    std::string* name;
    Id*          id;
    std::vector<Id*>*   ids;
    std::vector<Exps*>* args;
}

//TODO Destructors of discarded symbols
// %destructor { } <intlit>  //TODO Include types with no destructor defined
// %destructor { delete $$; } <*>  // Default destructor for type defined symbols

%% /* Production Rules */

program:
    "program" "name" "(" ")" ";" functions declarations compound_statement "." {
        ast_root = new Program($6, $7, $8);
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
        $$ = new Functions();
    }
    ;

function:
    "function" id_dcl
    {
        NameScope* namesp = identifiers::AddNameScope(ast::kCronological);
    }
    "(" "const" ids_dcl ":" type ")"
    {
        // Declaration of arguments
        for (Id* id : *$6) {
            id->RegisterAsVariable($8);
        }
    }
    ":" type  declarations compound_statement {
        std::vector<std::pair<Id*, Id*>> args;
        args.reserve($6->size());
        for (Id* id : *$6) {
            args.emplace_back($8, id);
        }
        $$ = new Function($12, $2, args, $13, $14);
        identifiers::AbandonCurrentNameScope();
        //TODO free $6
    }
    |
    error declarations compound_statement {
        
    }
    ;

declarations:
    declarations "var" ids_dcl ":" type ";" {
        $$ = $1;
        $$->AddIdentifiers(*$3, $5);
        // free($3);
    }
    |
    declarations "const" constants ";" {
        // $$ = $1;
        // $$->AddConstants($3);
        // free($3);
    }
    |
    {
        $$ = new Dcls();
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
        $$ = new Stmts();  //TODO
    }
    ;

statements:
    statements ";" statement {
        $$ = $1;
        $$->push_back($3);
    }
    |
    statement {
        $$ = new Stmts();
        $$->push_back($1);
    }
    ;

statement:
    assignment {
        $$ = $1;
    }
    |
    "if" expression "then" statement {
        $$ = new IfStmt($2, $4);
    }
    |
    "if" expression "then" statement
    "else" statement {
        $$ = new IfStmt($2, $4, $6);
    }
    |
    "while" expression "do" statement {
        $$ = new WhileStmt($2, $4);
    }
    | //TODO hay que añadir un namescope
    "for" id_dcl ":=" expression "to"
    expression "do" statement {
        $$ = new ForStmt($2, $4, $6, $8);
    }
    |
    "write" "(" print_list ")" {
        $$ = new WriteStmt($3);
    }
    |
    "read"  "(" read_list  ")" {
        $$ = new ReadStmt($3);
    }
    |
    compound_statement {
        $$ = $1;
    }
    ;

assignment:
    id_ref ":=" expression {
        $$ = new Assig($1, $3);
    }
    ;

expressions:
    expressions "," expression {
        $$ = $1;
        $$->push_back($3);
    }
    |
    expression {
        $$ = new Exps($1);
    }
    ;

expression:
    expression "+" expression {
        $$ = new BinOp(kPlus, $1, $3);
    }
    |
    expression "-" expression {
        $$ = new BinOp(kBinMinus, $1, $3);
    }
    |
    expression "*" expression {
        $$ = new BinOp(kAsterisk, $1, $3);
    }
    |
    expression "/" expression {
        $$ = new BinOp(kSlash, $1, $3);
    }
    |
    "-" expression %prec UMINUS {
        $$ = new UnaOp(kUnaMinus, $2);
    }
    |
    "(" expression ")" {
        $$ = $2;
    }
    | id_ref {
        $$ = $1;
    }
    |
    "int_lit" {
        $$ = new IntLit($1);
    }
    |
    id_ref "(" arguments ")" {
        $$ = new FuncCall($1, $3);
    }
    ;

arguments:
    expressions {
        $$ = $1;
    }
    |
    {
        //TODO
        $$ = new Exps();
    }
    ;

type:
    "integer" {
        $$ = identifiers::GetId("integer");
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
        $$ = new Exps();
        $$->push_back($1);
    }
    ;

print_item:
    expression {
        $$ = $1;
    }
    |
    "str_lit" {
        $$ = new StrLit($1);
    }
    ;

read_list:
    read_list "," read_item {
        $$ = $1;
        $$->push_back($3);
    }
    |
    read_item {
        $$ = new std::vector<Id*>();
        $$->push_back($1);
    }
    ;

read_item: //TODO considerar quitar ya que está name
    id_ref {
        $$ = $1;
    }
    ;

// Additions to the grammar
ids_dcl:
    ids_dcl "," id_dcl {
        $$ = $1;
        $$->push_back($3);
    }
    |
    id_dcl {
        $$ = new std::vector<Id*>();
        $$->push_back($1);
    }
    ;

id_dcl:
    "name" {
        $$ = identifiers::NewId(*$1);
        //TOOD free?
    }
    ;

id_ref:
    "name" {
        $$ = identifiers::GetId(*$1);
    }
    ;

%%

void yyerror(const char* msg) {
    printf("%d: ERROR: %s\n", yylineno, msg);
}
