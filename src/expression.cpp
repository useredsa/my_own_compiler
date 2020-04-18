#include "expression.hpp"

#include <iostream>
#include <string>
#include <vector>

namespace AST {

t_expressions::t_expressions() {    }

t_expressions::t_expressions(t_expression *first) {
    this->push_back(first);
}

void t_expressions::print(int lvl) {
    std::cout << std::string(lvl, '\t') << "expressions:\n";
    for (auto exp : *this) {
        exp->print(lvl+1);
    }
}

}  // namespace AST
