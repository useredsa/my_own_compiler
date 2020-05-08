#include "types.hpp"

using std::string;

namespace compiler {

namespace builtin {

void RegisterTypes() {
    new IntType(identifiers::GetId("int"));
    new StrType(identifiers::GetId("str"));
    new IntTypeBinPlus();
    new StrTypeBinPlus();
}

} // namespace builtin

} // namespace compiler

