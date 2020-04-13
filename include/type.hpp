#ifndef TYPE_HPP
#define TYPE_HPP

#include <iostream>
#include <string>

namespace AST {

class t_type {
  public:
    t_type() {  };

    virtual std::string llvm_name() = 0;

    void print(int lvl) {
        std::cout << std::string(lvl, '\t') << "type\n";
    }
};

}

#endif // TYPE_HPP

