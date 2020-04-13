#include "builtin.hpp"

#include "types.hpp"

namespace AST {

namespace builtin {

void llvm_register_builtins() {
    llvm_register_types();
}

} // namespace builtin

} // namespace AST

