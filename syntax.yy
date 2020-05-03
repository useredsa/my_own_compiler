%require "3.5"                     // Force Version >= 3.5 (at least 3.2 for C++)
%language "c++"                    // Instruct Bison to generate a C++ parse file
%defines                           // Create a header file
%define parse.error verbose        // Set verbose errors
%define api.value.type variant     // Use Bison's type-safe variant instead of a C union
                                   // for storing the data of rules. It also allows to 
                                   // use user-defined classes.
%define api.token.constructor      //TODO


%code top {
    /* Location(s): Near the top of the parser implementation file. It shouldn't
     * be unnecessary.
     */
}

%code requires {
    /* Location(s): The parser header file, included in the parser implementation
     * file before the Bison-generated YYSTYPE, YYLTYPE, token definitions and normal
     * %code blocks.
     */
    #include <iostream>
    #include <string>
    #include "ast.hpp"
    #include "types.hpp"
    namespace ast         = compiler::ast;
    namespace builtin     = compiler::builtin;
    namespace identifiers = compiler::identifiers;
    /* Flex needs this #define.
     * Bison needs the #define used.
     * We provide the define in the header and use it inside the cpp to declare
     * (not to implement) the function. Search: YY_DECL inside the `%code` block.
     */
    #define YY_DECL \
      yy::parser::symbol_type yylex ()
}

%code {
    /* Location(s): The parser implementation file after the usual contents of the
     * parser header file.
     */
    YY_DECL;
    extern int yylineno;

    ast::Program* ast_root;
    // void yyerror(const char* msg);
}




/* Grammar tokens */

    // Data Types
%token INT                             "int"
%token STR                             "str"
    // Literals
%token INTLIT                         "int_lit"
%token STRLIT                         "str_lit"
    // Keywords
%token PROGRAM                        "program"
%token FUNCTION                       "function"
%token CONST                          "const"
%token VAR                            "var"
%token BEGINN                         "begin"
%token END                            "end"
%token IF                             "if"
%token THEN                           "then"
%token ELSE                           "else"
%token WHILE                          "while"
%token DO                             "do"
%token FOR                            "for"
%token TO                             "to"
%token WRITE                          "write"
%token READ                           "read"
    // Operators
%token NAME                           "name"
%token SEMICOL                        ";"
%token COLON                          ":"
%token DOT                            "."
%token COMMA                          ","
%token PLUSOP                         "+"
%token MINUSOP                        "-"
%token MULTOP                         "*"
%token DIVOP                          "/"
%token LBRACKET                       "("
%token RBRACKET                       ")"
%token ASSIGNOP                       ":="

/* Nodes Types */
%type <int>                           "int_lit"
%type <std::string*>                  "str_lit"
%type <std::string*>                  "name"
%type <ast::Id*>                      id_dcl
%type <ast::Id*>                      id_ref
%type <ast::Id*>                      type
%type <std::vector<ast::Id*>*>        ids_dcl
%type <std::vector<ast::Id*>*>        read_list
%type <ast::Function*>                function
%type <std::vector<ast::Function*>*>  functions
%type <ast::Dcls*>                    declarations
%type <ast::Stmt>                     statement
%type <std::vector<ast::Stmt>*>       semcolon_sep_stmts_
%type <std::vector<ast::Stmt>*>       compound_statement
%type <std::vector<ast::Stmt>*>       semcolon_sep_stmts
%type <ast::Constants*>               constants
%type <ast::Exp>                      expression
%type <std::vector<ast::Exp>*>        print_list
%type <std::vector<ast::Exp>*>        comma_sep_exps
%type <std::vector<ast::Exp>*>        comma_sep_exps_




/* Grammar Definitions */
%start program                        // Start token
%right "then" "else"                  // Solution to the "dangling else" problem
                                      // Operator precedence
%left  "+" "-"                        // Least precedence
%left  "*" "/"                        // to
%right UMINUS                         // most precedence

//TODO Destructors of discarded symbols
// %destructor { } <intlit>  //TODO Include types with no destructor defined
// %destructor { delete $$; } <*>  // Default destructor for type defined symbols




/* Production Rules */
%%

program:
    "program" "name" "(" ")" ";" functions declarations compound_statement "." {
        ast_root = new ast::Program(std::move(*$6), $7, std::move(*$8));
        delete $6;
        delete $8;
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
        $$ = new std::vector<ast::Function*>();
    }
    ;

function:
    "function" id_dcl
    {
        identifiers::AddNameScope(ast::kCronological);
    }
    "(" "const" ids_dcl ":" type ")"
    {
        // Declaration of comma_sep_exps
        for (ast::Id* id : *$6) {
            id->RegisterAsVariable($8);
        }
    }
    ":" type declarations compound_statement {
        std::vector<std::pair<ast::Id*, ast::Id*>> args;
        args.reserve($6->size());
        for (ast::Id* id : *$6) {
            args.emplace_back($8, id);
        }
        $$ = new ast::Function($12, $2, args, $13, std::move(*$14));
        delete $6;
        delete $14;

        identifiers::AbandonCurrentNameScope();
    }
    |
    error declarations compound_statement {
        
    }
    ;

declarations:
    declarations "var" ids_dcl ":" type ";" {
        $$ = $1;
        $$->AddIdentifiers(*$3, $5);
        delete $3;
    }
    |
    declarations "const" constants ";" {
        //TODO $$ = $1;
        // $$->AddConstants($3);
        // delete $3;
    }
    |
    {
        $$ = new ast::Dcls();
    }
    ;

compound_statement:
    "begin" semcolon_sep_stmts "end" {
        $$ = $2;
    }
    ;

semcolon_sep_stmts:
    semcolon_sep_stmts_ {
        $$ = $1;
    }
    |
    {
        $$ = new std::vector<ast::Stmt>();  //TODO
    }
    ;

semcolon_sep_stmts_:
    semcolon_sep_stmts_ ";" statement {
        $$ = $1;
        $$->push_back($3);
    }
    |
    statement {
        $$ = new std::vector<ast::Stmt>();
        $$->push_back($1);
    }
    ;

statement:
    compound_statement {
        $$ = new ast::CompStmt(std::move(*$1));
        delete $1;
    }
    |
    id_ref ":=" expression {
        $$ = new ast::Assig($1, $3);
    }
    |
    "if" expression "then" statement {
        $$ = new ast::IfStmt($2, $4);
    }
    |
    "if" expression "then" statement "else" statement {
        $$ = new ast::IfStmt($2, $4, $6);
    }
    |
    "while" expression "do" statement {
        $$ = new ast::WhileStmt($2, $4);
    }
    | //TODO hay que añadir un namescope? o la variable debe estar ya declarada?
    "for" id_dcl ":=" expression "to"
    expression "do" statement {
        $$ = new ast::ForStmt($2, $4, $6, $8);
    }
    |
    "write" "(" print_list ")" {
        $$ = new ast::WriteStmt(std::move(*$3));
        delete $3;
    }
    |
    "read"  "(" read_list  ")" {
        $$ = new ast::ReadStmt(std::move(*$3));
        delete $3;
    }
    ;

comma_sep_exps:
    comma_sep_exps_ {
        $$ = $1;
    }
    |
    {
        $$ = new std::vector<ast::Exp>();
    }
    ;

comma_sep_exps_:
    comma_sep_exps_ "," expression {
        $$ = $1;
        $$->push_back($3);
    }
    |
    expression {
        $$ = new std::vector<ast::Exp>();
        $$->push_back($1);
    }
    ;

expression:
    expression "+" expression {
        $$ = new ast::BinOp<ast::kPlus>($1, $3);
    }
    |
    expression "-" expression {
        $$ = new ast::BinOp<ast::kBinMinus>($1, $3);
    }
    |
    expression "*" expression {
        $$ = new ast::BinOp<ast::kAsterisk>($1, $3);
    }
    |
    expression "/" expression {
        $$ = new ast::BinOp<ast::kSlash>($1, $3);
    }
    |
    //%prec Treat this '-' with the precedence of UMINUS
    "-" expression %prec UMINUS {
        $$ = new ast::UnaOp<ast::kUnaMinus>($2);
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
        $$ = new ast::IntLit($1);
    }
    |
    "str_lit" {
        $$ = new ast::StrLit($1);
    }
    |
    id_ref "(" comma_sep_exps ")" {
        $$ = new ast::FuncCall($1, std::move(*$3));
        delete $3;
    }
    ;

type:
    "int" {
        $$ = builtin::IntTypeId();
    }
    |
    "str" {
        $$ = builtin::StrTypeId();
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
    print_list "," expression {
        $$ = $1;
        $$->push_back($3);
    }
    |
    expression {
        $$ = new std::vector<ast::Exp>();
        $$->push_back($1);
    }
    ;

read_list:
    read_list "," id_ref {
        $$ = $1;
        $$->push_back($3);
    }
    |
    id_ref {
        $$ = new std::vector<ast::Id*>();
        $$->push_back($1);
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
        $$ = new std::vector<ast::Id*>();
        $$->push_back($1);
    }
    ;

id_dcl:
    "name" {
        $$ = identifiers::NewId(std::move(*$1));
        delete $1;
    }
    ;

id_ref:
    "name" {
        $$ = identifiers::GetId(std::move(*$1));
        delete $1;
    }
    ;

%%

void yy::parser::error(const std::string& m) {
    std::cerr << "ERROR: " << m << '\n';
}

