%require "3.4"                     // Force Version >= 3.4 (at least 3.2 for C++)
%language "c++"                    // Instruct Bison to generate a C++ parse file
%defines                           // Create a header file
%define parse.error verbose        // Set verbose errors
%define api.value.type variant     // Use Bison's type-safe variant instead of a C union
                                   // for storing the data of rules. It also allows to 
                                   // use user-defined classes.
%define api.token.constructor      // When variant-based semantic values are enabled, request
                                   // that symbols be handled as a whole (type, value, and
                                   // possibly location) in the scanner.

%code top {
    /* Location(s): Near the top of the parser implementation file. It shouldn't
     * be necessary.
     */
}

%code requires {
    /* Location(s): The parser header file, included in the parser implementation
     * file before the Bison-generated YYSTYPE, YYLTYPE, token definitions and normal
     * %code blocks.
     */
    #include <iostream>
    #include <string>
    
    #include "log.hpp"
    #include "ast.hpp"
    #include "builtin.hpp"
    #include "statements.hpp"
    #include "expressions.hpp"

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
%token OPERATOR                               "operator"
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
%type <std::string*>                          program_header
%type <identifiers::Id*>                      new_id
%type <std::vector<identifiers::Id*>*>        comma_sep_dcl
%type <identifiers::Id*>                      id_ref
%type <identifiers::Id*>                      function_id
%type <ast::RType>                            rtype
%type <ast::RVar>                             rvar
%type <ast::RFun>                             rfun
%type <std::vector<ast::RVar>*>               comma_sep_rvar
%type <std::vector<ast::RVar>*>               comma_sep_rvar_
%type <ast::Fun*>                             function
%type <std::vector<ast::Fun*>*>               functions
%type <ast::Var*>                             single_arg
%type <std::vector<ast::Var*>*>               optional_args
%type <std::vector<ast::Var*>*>               args
%type <std::vector<ast::Var*>*>               declarations
%type <std::vector<ast::Var*>*>               constants
%type <ast::Stmt>                             statement
%type <std::vector<ast::Stmt>*>               semcolon_sep_stmts_
%type <std::vector<ast::Stmt>*>               compound_statement
%type <std::vector<ast::Stmt>*>               semcolon_sep_stmts
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



/* Production Rules */
%%

program:
    program_header[name] functions
	{
    	identifiers::AddNameScope(identifiers::kCronological);
        identifiers::NewId(".main");
	}
    declarations compound_statement "." {
        $declarations->push_back(new ast::Var(identifiers::GetId(".main"),
                                              ast::RType(builtin::IntTypeId()),
                                              ast::Exp(new ast::IntLit(0))));
        identifiers::AbandonCurrentNameScope();
        ast_root = new ast::Prog(std::move(*$name),
                                 std::move(*$functions),
                                 std::move(*$declarations),
                                 std::move(*$compound_statement));
        delete $name;
        delete $functions;
        delete $declarations;
        delete $compound_statement;
    }
    |
    // errors
    error ";" functions
    {
    	identifiers::AddNameScope(identifiers::kCronological);
        identifiers::NewId(".main");
    }
    declarations compound_statement "." {
        $declarations->push_back(new ast::Var(identifiers::GetId(".main"),
                                              ast::RType(builtin::IntTypeId()),
                                              ast::Exp(new ast::IntLit(0))));
        ast_root = new ast::Prog(std::move(std::string(".error_program")),
                                 std::move(*$functions),
                                 std::move(*$declarations),
                                 std::move(*$compound_statement));
        delete $functions;
        delete $declarations;
        delete $compound_statement;
    }
    ;

program_header:
    "program" "name"[name] "(" ")" ";" {
        $$ = $name;
    }
    |
    "program" "name"[name] "(" ")" {
        yy::parser::error("syntax error, expecting \";\" at the end of program header");
        $$ = $name;
    }

functions:
    functions function ";" {
        $$ = $1;
        $$->push_back($2);
    }
    |
    {
        $$ = new std::vector<ast::Fun*>();
    }
    |
    // errors
    functions function {
        $$ = $1;
        $$->push_back($2);
        yy::parser::error("syntax error, expecting \";\" at the end of function definition");
        yyerrok;
    }
    ;

function:
    "function" function_id[fun_id]
    {
        identifiers::AddNameScope(identifiers::kCronological);
        switch ($fun_id->name().back()) {
          case static_cast<char>(ast::kPlus):
          case static_cast<char>(ast::kBinMinus):
          case static_cast<char>(ast::kAsterisk):
          case static_cast<char>(ast::kSlash):
              identifiers::NewId("op");
              break;
          default:
              identifiers::NewId(std::string($fun_id->name()));
        }
    }
    "(" optional_args ")" ":" rtype[return_type] declarations compound_statement {
        switch ($fun_id->name().back()) {
          case static_cast<char>(ast::kPlus):
          case static_cast<char>(ast::kBinMinus):
          case static_cast<char>(ast::kAsterisk):
          case static_cast<char>(ast::kSlash): {
              if ($optional_args->size() != 2)
                  compiler::semantic_log
                    << "Binary operator must be overloaded with exactly 2 arguments\n";
              }
              $declarations->push_back(new ast::Var(identifiers::GetId("op"),
                                                    $return_type));
              break;
          default:
              $declarations->push_back(new ast::Var(
                                         identifiers::GetId(std::string($fun_id->name())),
                                                            $return_type));
        }
        $$ = new ast::Fun($fun_id,
                          $return_type,
                          std::move(*$optional_args),
                          std::move(*$declarations),
                          std::move(*$compound_statement));
        identifiers::AbandonCurrentNameScope();
        delete $optional_args;
        delete $declarations;
        delete $compound_statement;
    }
    |
    // errors
    "function" error declarations compound_statement {
        $$ = builtin::ErrorFun();
        delete $declarations;
        delete $compound_statement;
    }
    ;

optional_args:
    args {
        $$ = $1;
    }
    |
    {
        $$ = new std::vector<ast::Var*>();
    }

args:
    args "," single_arg {
        $$ = $1;
        $$->push_back($3);
    }
    |
    single_arg {
        $$ = new std::vector<ast::Var*>();
        $$->push_back($1);
    }
    
single_arg:
    "const" new_id ":" rtype {
        $$ = new ast::Var($2, $4, ast::Exp(new ast::NoExp), true);
    }
    |
    new_id ":" rtype {
        $$ = new ast::Var($1, $3);
    }

declarations:
    declarations "var" comma_sep_dcl[ids] ":" rtype ";" {
        $$ = $1;
        for (identifiers::Id* id : *$ids) {
            $$->push_back(new ast::Var(id, $rtype));
        }
        delete $ids;
    }
    |
    declarations "const" constants ";" {
        $$ = $1;
        $$->insert($$->end(), $3->begin(), $3->end());
        delete $3;
    }
    |
    {
        $$ = new std::vector<ast::Var*>();
    }
    |
    // errors  //IMPROVEMENT Avoid discarding declarations previous to a declaration without var nor const
    declarations "var" error ";" {
        $$ = $1;
        yyerrok;
    }
    |
    declarations "const" error ";" {
        $$ = $1;
        yyerrok;
    }
    |
    declarations "var" comma_sep_dcl[ids] ":" rtype {
        yy::parser::error("syntax error, expecting \";\"");
        $$ = $1;
        for (identifiers::Id* id : *$ids) {
            $$->push_back(new ast::Var(id, $rtype));
        }
    }
    |
    declarations "const" constants {
        yy::parser::error("syntax error, expecting \";\"");
        $$ = $1;
        $$->insert($$->end(), $3->begin(), $3->end());
        delete $3;
    }
    |
    declarations comma_sep_dcl[ids] ":" rtype ";" {
        yy::parser::error("syntax error, expecting \"var\" in variable declarations");
        $$ = $1;
        for (identifiers::Id* id : *$ids) {
            $$->push_back(new ast::Var(id, $rtype));
        }
    }
    ;

constants:
    constants "," new_id ":=" expression {
        $$ = $1;
        $$->push_back(new ast::Var($3, ast::RType(builtin::IntTypeId()), $5, true));
    }
    |
    new_id ":=" expression {
        $$ = new std::vector<ast::Var*>();
        $$->push_back(new ast::Var($1, ast::RType(builtin::IntTypeId()), $3, true));
    }
    ;

compound_statement:
    "begin" semcolon_sep_stmts "end" {
        $$ = $2;
    }
    |  
    // errors
    "begin" error "end" {  // This rule parses an all-errors compund_statement
        $$ = new std::vector<ast::Stmt>();
        yyerrok;
    }
    ;

semcolon_sep_stmts:
    semcolon_sep_stmts_ {
        $$ = $1;
    }
    |
    {
        $$ = new std::vector<ast::Stmt>();
    }
    |
    // errors
    semcolon_sep_stmts_ ";" {
        yy::parser::error("syntax error, unexpected \";\", expecting \"end\"");
        $$ = $1;
    }
    ;

semcolon_sep_stmts_:
    semcolon_sep_stmts_ ";" {yyerrok;} statement {
        $$ = $1;
        $$->push_back($4);
    }
    |
    statement {
        $$ = new std::vector<ast::Stmt>();
        $$->push_back($1);
    }
    |
    // errors
    semcolon_sep_stmts_ error {
        $$ = $1;
    }
    |
    error ";" {yyerrok;} statement {
        $$ = new std::vector<ast::Stmt>();
        $$->push_back($4);
        yyerrok;
    }
    |
    semcolon_sep_stmts_ statement {
        yy::parser::error("syntax error, expecting \";\" between statements");
        $$ = $1;
        $$->push_back($statement);
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
    "if" expression[cond] "then" statement[stmt] {
        $$ = new ast::IfStmt($cond, $stmt);
    }
    |
    "if" expression[cond] "then" statement[stmt] "else" statement[alt_stmt] {
        $$ = new ast::IfStmt($cond, $stmt, $alt_stmt);
    }
    |
    "while" expression[cond] "do" statement[stmt] {
        $$ = new ast::WhileStmt($cond, $stmt);
    }
    |
    "for" rvar[ctrl_var] ":=" expression[init_exp] "to"
    expression[final_exp] "do" statement[body] {
        $$ = new ast::ForStmt($ctrl_var, $init_exp, $final_exp, $body);
    }
    |
    "write" "(" print_list ")" {
        $$ = new ast::WriteStmt(std::move(*$print_list));
        delete $print_list;
    }
    |
    "read" "(" comma_sep_rvar[read_list] ")" {
        $$ = new ast::ReadStmt(std::move(*$read_list));
        delete $read_list;
    }
    |
    // errors
    "if" expression[cond] statement[stmt]  %prec "then" {
        yy::parser::error("syntax error, expecting \"then\" in if statement");
        $$ = new ast::IfStmt($cond, $stmt);
    }
    |
    "if" expression[cond] statement[stmt] "else" statement[alt_stmt]  %prec "else" {
        yy::parser::error("syntax error, expecting \"then\" in if statement");
        $$ = new ast::IfStmt($cond, $stmt, $alt_stmt);
    }
    |
    "while" expression[cond] statement[stmt] {
        yy::parser::error("syntax error, expecting \"do\" in while statement");
        $$ = new ast::WhileStmt($cond, $stmt);
    }
    |
    "for" rvar[ctrl_var] ":=" error statement[stmt] {
        $$ = $stmt;
    }
    |
    "write" "(" error ")" {
        $$ = new ast::EmptyStmt();
        yyerrok;
    }
    |
    "read" "(" error ")" {
        $$ = new ast::EmptyStmt();
        yyerrok;
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
    |
    rvar {
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
    |
    // errors
    "(" error ")" {
        $$ = new ast::NoExp();
        yyerrok;
    }
    |
    rfun "(" error ")" {
        $$ = new ast::NoExp();
        yyerrok;
    }
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
    |
    // errors
    error {
        $$ = ast::RType(builtin::ErrorType()->id());
        yyclearin;
        yyerrok;
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

function_id:
    id_ref {
        $$ = $1;
    }
    |
    "+" "operator" {
        $$ = identifiers::GetId(".operator+");
    }
    |
    "-" "operator" {
        $$ = identifiers::GetId(".operator-");
    }
    |
    "*" "operator" {
        $$ = identifiers::GetId(".operator*");
    }
    |
    "/" "operator" {
        $$ = identifiers::GetId(".operator/");
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
    std::string e = m;
    if (m.substr(0,14) == "syntax error, ")
        e = m.substr(14);
    compiler::syntactic_log << yylineno << ": " << e << '\n';
}
