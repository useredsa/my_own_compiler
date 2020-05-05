#include "types.hpp"

using std::string;

namespace compiler {

namespace builtin {

void RegisterTypes() {
    identifiers::GetId("int")->RegisterAsType(new IntType());
    // new IntTypeBinPlus(); //TODO las funciones se registran solas lo cual es raro
    // new StrTypeBinPlus();
    
    identifiers::GetId("str")->RegisterAsType(new StrType());
}

} // namespace builtin

} // namespace compiler

