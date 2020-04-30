#include "expression.hpp"

#include <iostream>
#include <string>
#include <vector>

namespace compiler {

namespace ast {

Exps::Exps(IExp *first) {
    this->push_back(first);
}

void Exps::print(int lvl) {
    std::cout << std::string(lvl, '\t') << "expressions:\n";
    for (auto exp : *this) {
        exp->print(lvl+1);
    }
}

} // namespace ast

} // namespace compiler

