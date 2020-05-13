#include <getopt.h>

#include <iostream>
#include <fstream>
#include <cstdio>

#include "syntax.tab.hpp"
#include "builtin.hpp"
#include "ast-printer.hpp"
#include "llvm.hpp"
#include "log.hpp"
#include "id_resolution.hpp"

extern ast::Prog* ast_root;

bool        PRINT_AST = false;
std::string INPUT_FILE;
std::string OUTPUT_FILE;

/**
 * @brief Option processing
 */
int options(int argc, char* argv[]) {
    bool custom_output = false;
    int opt;
    optind = 1;
    while ((opt = getopt(argc, argv, "tho:")) != -1) {
        switch (opt) {
            case 'o':
                if (custom_output) {
                    std::cerr << "mpc: Cannot set two output files\n";
                    exit(-1);
                }
                custom_output = true;
                OUTPUT_FILE = optarg;
                break;

            case 't':
                PRINT_AST = true;
                break;

            case 'h':
                std::cout <<
                    "mpc: Usage: mpc [-o OUT] [-t] [-h] FILE\n"
                    "     -o OUT Set output file. Defaults to FILE with the extension .llvm\n"
                    "     -t     Print abstract syntax tree\n"
                    "     -h     Print this information\n\n";
                exit(0);

            default:
            case '?':
            case ':':
                std::cout << "mpc: Usage: mpc [-o OUT] [-t] [-h] FILE\n";
                exit(-1);
        }
    }
    if (optind+1 != argc) {
        std::cerr << "mpc: Must input exactly one file\n";
        exit(-1);
    }
    //NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic): necessary
    INPUT_FILE = argv[optind];

    if (custom_output == false) {
        OUTPUT_FILE = INPUT_FILE;
        size_t pos = OUTPUT_FILE.rfind('.');
        size_t name_init = OUTPUT_FILE.rfind('/');
        if (pos == OUTPUT_FILE.npos or pos < name_init) {
            pos = OUTPUT_FILE.size();
        }
        OUTPUT_FILE.resize(pos);
        OUTPUT_FILE += ".llvm";
    }
    return 0;
}

int main(int argc, char* argv[]) {
    options(argc, argv);
    //IMPROVEMENT open in a ifstream and pass it to a program class.
    freopen(INPUT_FILE.c_str(), "r", stdin);

    // Initialization needed
    compiler::builtin::RegisterBuiltins();
    
    // Parser object
    yy::parser parse{};
    // Parsing
    if (parse() > 0) {
        std::cout << "The program terminated due to syntactic error(s)\n";
        exit(-11);
    }

    if (PRINT_AST) {
        compiler::AstPrinter printer(std::cout);
        printer(ast_root);
        std::cout.flush();
    }

    compiler::identifiers::NameResolution::Do();
    compiler::identifiers::NameResolution name_resolver;
    name_resolver(ast_root);
    if (compiler::semantic_log.GetCounter() > 0 or
        compiler::syntactic_log.GetCounter() > 0) {
        std::cout << "The program terminated due to error(s)\n";
        std::cout << "Number of lexical errors: "
                  << compiler::lexical_log.GetCounter() << "\n";
        std::cout << "Number of syntactic errors: "
                  << compiler::syntactic_log.GetCounter() << "\n";
        std::cout << "Number of semantic errors: "
                  << compiler::semantic_log.GetCounter() << "\n";
        exit(-12);
    }

    std::ofstream os(OUTPUT_FILE);
    compiler::llvm::Translator translator(os);
    translator.Output(ast_root);
}

