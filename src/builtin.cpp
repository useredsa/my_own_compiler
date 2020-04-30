#include "builtin.hpp"

#include "types.hpp"

namespace compiler {

namespace ast {

namespace builtin {

void RegisterBuiltins() {
    identifiers::AddNameScope(kAcronological);
    RegisterTypes();
    identifiers::NewId("main");
}

} // namespace builtin

} // namespace ast

} // namespace compiler

