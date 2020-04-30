#ifndef TYPE_HPP
#define TYPE_HPP

#include <iostream>
#include <string>

namespace compiler {

namespace ast {

class Type {
  public:
    Type() {  };

    virtual const std::string& llvm_name() = 0;

    void print(int lvl) {
        std::cout << std::string(lvl, '\t') << "type\n";
    }
};

} // namespace ast

} // namespace compiler

#endif // TYPE_HPP

