%require "3.4"                     // Force Version >= 3.4 (at least 3.2 for C++)
%language "c++"                    // Instruct Bison to generate a C++ parse file
%defines                           // Create a header file
%define parse.error verbose        // Set verbose errors
%define api.value.type variant     // Use Bison's type-safe variant instead of a C union
                                   // for storing the data of rules. It also allows to 
                                   // use user-defined classes.
%define api.token.constructor      //TODO


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
    "program" "name"[name] "(" ")" ";" functions declarations compound_statement "." {
        ast_root = new ast::Prog(std::move(*$name),
                                 std::move(*$functions),
                                 std::move(*$declarations),
                                 std::move(*$compound_statement));
        delete $name;
        delete $functions;
        delete $declarations;
        delete $compound_statement;
        if (yynerrs_) YYABORT;
    }
    |
    error ";" { yyerrok; } functions declarations compound_statement "." {
        //TODO
        if (yynerrs_) YYABORT;
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
    |
    // errors
    functions function { //TODO Podría ser un warning
        $$ = $1;
        $$->push_back($2);
        yy::parser::error("syntax error, expecting \";\"");
        yyerrok;
    }
    ;

function:
    "function" id_ref[fun_name]
    {
        identifiers::AddNameScope(identifiers::kCronological);
    }
    "(" "const" comma_sep_dcl[args] ":" rtype[args_type] ")"
    {
        // Declaration of comma_sep_ids
        for (identifiers::Id* id : *$args) {
            new ast::Var(id, $args_type);
        }
    }
    ":" rtype[return_type] declarations compound_statement {
        std::vector<ast::Var*> args;
        args.reserve($args->size());
        for (identifiers::Id* id : *$args) {
            args.push_back(id->var());
        }
        $$ = new ast::Fun($fun_name,
                          $return_type,
                          std::move(args),
                          std::move(*$declarations),
                          std::move(*$compound_statement));
        identifiers::AbandonCurrentNameScope();
        delete $args;
        delete $declarations;
        delete $compound_statement;
    }
    |
    "function" error declarations compound_statement {
        //TODO
    }
    ;

declarations:
    declarations "var" comma_sep_dcl ":" rtype ";" {
        $$ = $1;
        for (identifiers::Id* id : *$3) {
            $$->push_back(new ast::Var(id, $5));
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
    |  // errors
    declarations "var" error ";" {
        yyerrok;
    }
    |
    declarations "const" error ";" {
        yyerrok;
    }
    |
    declarations "var" comma_sep_dcl ":" rtype {
        yy::parser::error("syntax error, expecting \";\"");
        yyerrok;
    }
    |
    declarations "const" constants {
        yy::parser::error("syntax error, expecting \";\"");
        yyerrok;
    }
    ;

compound_statement:
    "begin" semcolon_sep_stmts "end" {
        $$ = $2;
    }
    |
    "begin" error "end" {
        yyerrok;
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
    |
    // errors
    error ";" { yyerrok; } statement {
        //TODO No cargarse todos los tokens anteriores al error
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
    | //TODO hay que añadir un namescope? o la variable debe estar ya declarada? Imagino que lo primero
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
    "if" expression[cond] statement[stmt] {
        $$ = new ast::EmptyStmt(); //TODO
        yy::parser::error("syntax error, expecting \"then\"");
    }
    |
    "while" expression[cond] statement[stmt] {
        $$ = new ast::EmptyStmt(); //TODO
        yy::parser::error("syntax error, expecting \"do\"");
    }
    |
    "for" rvar[ctrl_var] ":=" error statement[stmt] {
        $$ = new ast::EmptyStmt(); //TODO
    }
    |
    "write" "(" error ")" {
        $$ = new ast::EmptyStmt(); //TODO
    }
    |
    "read" "(" error ")" {
        $$ = new ast::EmptyStmt(); //TODO
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
    "(" error ")" {
        yyerrok;
    }
    |
    rfun "(" error ")" {
        yyerrok;
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
    std::cerr << yylineno << ": ERROR: " << m << '\n';
}

