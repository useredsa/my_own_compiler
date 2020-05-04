#include "builtin.hpp"

#include "types.hpp"
#include "expressions.hpp"

namespace compiler {

namespace builtin {

void RegisterBuiltins() {
    identifiers::AddNameScope(identifiers::kAcronological);
    RegisterTypes();
    identifiers::NewId("main");
    new ast::StrLit(new std::string("%d"), "@.io.int");
    new ast::StrLit(new std::string("%s"), "@.io.str");
}

} // namespace builtin

} // namespace compiler

