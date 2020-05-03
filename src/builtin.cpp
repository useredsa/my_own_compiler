#include "builtin.hpp"

#include "types.hpp"
#include "expressions.hpp"

namespace compiler {

namespace builtin {

void RegisterBuiltins() {
    identifiers::AddNameScope(ast::kAcronological);
    RegisterTypes();
    identifiers::NewId("main");
    new ast::StrLit(new std::string("%d"), "@.io.int");
    new ast::StrLit(new std::string("%s"), "@.io.str");
}

void LlvmPutStrLits(std::ostream& os) {
    for (ast::StrLit* lit : ast::program_str_lits) {
        os << lit->llvm_id << " = private unnamed_addr constant [" << lit->lit->size()+1
           << " x i8] c\"" << *lit->lit << "\\00\", align 1\n";
    }
}

} // namespace builtin

} // namespace compiler

