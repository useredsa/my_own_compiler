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
    #include "statements.hpp"
    #include "expressions.hpp"
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

    ast::Prog* ast_root;
    // void yyerror(const char* msg);
}




/* Grammar tokens */

    // Data Types
%token INT                                    "int"
%token STR                                    "str"
    // Literals
%token INTLIT                                 "int_lit"
%token STRLIT                                 "str_lit"
    // Keywords
%token PROGRAM                                "program"
%token FUNCTION                               "function"
%token CONST                                  "const"
%token VAR                                    "var"
%token BEGINN                                 "begin"
%token END                                    "end"
%token IF                                     "if"
%token THEN                                   "then"
%token ELSE                                   "else"
%token WHILE                                  "while"
%token DO                                     "do"
%token FOR                                    "for"
%token TO                                     "to"
%token WRITE                                  "write"
%token READ                                   "read"
    // Operators
%token NAME                                   "name"
%token SEMICOL                                ";"
%token COLON                                  ":"
%token DOT                                    "."
%token COMMA                                  ","
%token PLUSOP                                 "+"
%token MINUSOP                                "-"
%token MULTOP                                 "*"
%token DIVOP                                  "/"
%token LBRACKET                               "("
%token RBRACKET                               ")"
%token ASSIGNOP                               ":="

/* Nodes Types */
%type <int>                                   "int_lit"
%type <std::string*>                          "str_lit"
%type <std::string*>                          "name"
%type <identifiers::Id*>                      new_id
%type <std::vector<identifiers::Id*>*>        comma_sep_dcl
%type <identifiers::Id*>                      id_ref
%type <ast::RType>                            rtype
%type <ast::RVar>                             rvar
%type <ast::RFun>                             rfun
%type <std::vector<ast::RVar>*>               comma_sep_rvar
%type <std::vector<ast::RVar>*>               comma_sep_rvar_
%type <ast::Fun*>                             function
%type <std::vector<ast::Fun*>*>               functions
%type <std::vector<ast::Var*>*>               declarations
%type <ast::Stmt>                             statement
%type <std::vector<ast::Stmt>*>               semcolon_sep_stmts_
%type <std::vector<ast::Stmt>*>               compound_statement
%type <std::vector<ast::Stmt>*>               semcolon_sep_stmts
// %type <ast::Constants*>                       constants
%type <ast::Exp>                              expression
%type <std::vector<ast::Exp>*>                print_list
%type <std::vector<ast::Exp>*>                comma_sep_exps
%type <std::vector<ast::Exp>*>                comma_sep_exps_




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
        ast_root = new ast::Prog(std::move(*$2),
                                 std::move(*$6),
                                 std::move(*$7),
                                 std::move(*$8));
        delete $2;
        delete $6;
        delete $7;
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
        $$ = new std::vector<ast::Fun*>();
    }
    ;

function:
    "function" id_ref
    {
        identifiers::AddNameScope(identifiers::kCronological);
    }
    "(" "const" comma_sep_dcl ":" rtype ")"
    {
        // Declaration of comma_sep_exps
        for (identifiers::Id* id : *$6) {
            id->RegisterAsVariable(new ast::Var(id, $8));
        }
    }
    ":" rtype declarations compound_statement {
        std::vector<ast::Var*> args;
        args.reserve($6->size());
        for (identifiers::Id* id : *$6) {
            args.push_back(id->var());
        }
        $$ = new ast::Fun($2, $12, std::move(args), std::move(*$13), std::move(*$14));
        identifiers::AbandonCurrentNameScope();
        delete $6;
        delete $13;
        delete $14;
    }
    |
    error declarations compound_statement {
        
    }
    ;

declarations:
    declarations "var" comma_sep_dcl ":" rtype ";" {
        $$ = $1;
        for (identifiers::Id* id : *$3) {
            auto* var = new ast::Var(id, $5);
            id->RegisterAsVariable(var);
            $$->push_back(var);
        }
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
        $$ = new std::vector<ast::Var*>();
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
    rvar ":=" expression {
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
    "for" rvar ":=" expression "to"
    expression "do" statement {
        $$ = new ast::ForStmt($2, $4, $6, $8);
    }
    |
    "write" "(" print_list ")" {
        $$ = new ast::WriteStmt(std::move(*$3));
        delete $3;
    }
    |
    "read"  "(" comma_sep_rvar_  ")" {
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
    | rvar {
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
    rfun "(" comma_sep_exps ")" {
        $$ = new ast::FunCall($1, std::move(*$3));
        delete $3;
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

comma_sep_rvar:
    comma_sep_rvar_ {
        $$ = $1;
    }
    |
    {
        $$ = new std::vector<ast::RVar>();
    }

comma_sep_rvar_:
    comma_sep_rvar_ "," rvar {
        $$ = $1;
        $$->push_back($3);
    }
    |
    rvar {
        $$ = new std::vector<ast::RVar>();
        $$->push_back($1);
    }
    ;

// Additions to the grammar
rtype:
    "int" {
        $$ = ast::RType(builtin::IntTypeId());
    }
    |
    "str" {
        $$ = ast::RType(builtin::StrTypeId());
    }
    ;

rvar:
    id_ref {
        $$ = ast::RVar($1);
    }
    ;

rfun:
    id_ref {
        $$ = ast::RFun($1);
    }
    ;

comma_sep_dcl:
    comma_sep_dcl "," new_id {
        $$ = $1;
        $$->push_back($3);
    }
    |
    new_id {
        $$ = new std::vector<identifiers::Id*>();
        $$->push_back($1);
    }
    ;

new_id:
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

