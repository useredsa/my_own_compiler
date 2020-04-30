#include "types.hpp"

#include "namespace.hpp"
#include "function.hpp"

using std::string;

namespace compiler {

namespace ast {

namespace builtin {

const string IntType::llvm_name_ = "i32";

class IntTypeBinPlus : public Function {
  public:
    IntTypeBinPlus() : Function(IntTypeId(),
                                identifiers::GetId("operator+"),
                                {{IntTypeId(), identifiers::GetId(".lhs")},
                                 {IntTypeId(), identifiers::GetId(".rhs")}},
                                nullptr,
                                nullptr) {  }
    
    virtual string llvm_put_call(std::ostream& os,
                                 int& local_var_count,
                                 std::vector<string*> params) {
        assert(params.size() == 2);
        string ref = "%" + std::to_string(local_var_count++);
        os << "\t" << ref << " = add nsw i32 " << *params[0] << ", " 
           << *params[1] << "\n";
        return ref;
    }
};

const string StrType::llvm_name_ = "i8*";

void RegisterTypes() {
    identifiers::GetId("integer")->RegisterAsType(new IntType()); //TODO por dios que alguien llame int a integer!!!!
    new IntTypeBinPlus(); //TODO las funciones se registran solas lo cual es raro
    
    identifiers::GetId("str")->RegisterAsType(new StrType());
}

} // namespace builtin

} // namespace ast

} // namespace compiler

