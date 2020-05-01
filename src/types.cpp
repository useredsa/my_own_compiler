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

class StrTypeBinPlus : public Function {
  public:
    StrTypeBinPlus() : Function(StrTypeId(),
                                identifiers::GetId("operator+"),
                                {{StrTypeId(), identifiers::GetId(".lhs")},
                                 {StrTypeId(), identifiers::GetId(".rhs")}},
                                nullptr,
                                nullptr) {  }
    
    virtual string llvm_put_call(std::ostream& os,
                                 int& local_var_count,
                                 std::vector<string*> params) {
        assert(params.size() == 2);
        string len1 = "%" + std::to_string(local_var_count++);
        string len2 = "%" + std::to_string(local_var_count++);
        os << "\t" << len1 << " = call i64 @strlen(i8* " << *params[0] << ")\n";
        os << "\t" << len2 << " = call i64 @strlen(i8* " << *params[1] << ")\n";
        string lenNoNull = "%" + std::to_string(local_var_count++);
        os << "\t" << lenNoNull << " = add nsw i64 " << len1 << ", " << len2 << "\n";
        string len = "%" + std::to_string(local_var_count++);
        os << "\t" << len << " = add nsw i64 " << lenNoNull << ", 1" << "\n";
        string ref = "%" + std::to_string(local_var_count++);
        
        os << "\t" << ref << " = alloca i8, i64 " << len << "\n";
        
        os << "\t%" << local_var_count++ << " = call i8* @strcpy(i8* " << ref << ", "
           << "i8*" << *params[0] << ")\n";
        string end = "%" + std::to_string(local_var_count++);
        os << "\t" << end << " = getelementptr i8, i8* " << ref << ", i64 " << len1 << "\n";
        os << "\t%" << local_var_count++ << " = call i8* @strcpy(i8* " << end << ", "
           << "i8*" << *params[1] << ")\n";
        return ref;
    }
};

void RegisterTypes() {
    identifiers::GetId("integer")->RegisterAsType(new IntType()); //TODO por dios que alguien llame int a integer!!!!
    new IntTypeBinPlus(); //TODO las funciones se registran solas lo cual es raro
    new StrTypeBinPlus();
    
    identifiers::GetId("str")->RegisterAsType(new StrType());
}

} // namespace builtin

} // namespace ast

} // namespace compiler

