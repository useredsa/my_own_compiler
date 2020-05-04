#include "types.hpp"

#include "function.hpp"

using std::string;

namespace compiler {

namespace builtin {

void RegisterTypes() {
    identifiers::GetId("integer")->RegisterAsType(new IntType());
    // new IntTypeBinPlus(); //TODO las funciones se registran solas lo cual es raro
    // new StrTypeBinPlus();
    
    identifiers::GetId("str")->RegisterAsType(new StrType());
}

} // namespace builtin

} // namespace compiler

