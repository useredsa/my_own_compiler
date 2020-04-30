#include "statement.hpp"

#include <iostream>

namespace compiler {

namespace ast {

void Stmts::llvm_put(std::ostream& os, int& local_var_count) {
    //TODO
    for (auto st : *this) {
        st->llvm_put(os, local_var_count);
    }
}

void Stmts::print(int lvl) {
    std::string tabs(lvl, '\t');
    std::cout << tabs << "begin\n";
    for (auto st : *this) {
        st->print(lvl+1);
    }
    std::cout << tabs << "end\n";
}

} // namespace ast

} // namespace compiler

