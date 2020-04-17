#include "statement.hpp"

#include <iostream>

namespace AST {

void t_statements::llvm_put(std::ostream& os, int& local_var_count) {
    //TODO
    for (auto st : *this) {
        st->llvm_put(os, local_var_count);
    }
}

void t_statements::print(int lvl) {
    std::string tabs(lvl, '\t');
    std::cout << tabs << "begin\n";
    for (auto st : *this) {
        st->print(lvl+1);
    }
    std::cout << tabs << "end\n";
}

}  // namespace ASTs
