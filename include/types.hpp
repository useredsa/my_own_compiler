#ifndef TYPES_HPP
#define TYPES_HPP

#include <iostream>
#include <string>

namespace AST {

namespace builtin {
    
    class t_int : public t_type { //TODO transform to singleton?
        std::string llvm_name() {
            return "i32";
        }
    };

    class t_str : public t_type {
        std::string llvm_name() {
            return "i8*";
        }
    };

}

}

#endif // TYPES_HPP

