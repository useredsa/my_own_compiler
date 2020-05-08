#ifndef TYPES_HPP
#define TYPES_HPP

#include <iostream>
#include <string>

#include "ast_defs.hpp"
#include "ast.hpp"
#include "identifiers.hpp"

using std::string;

namespace compiler {

namespace builtin {

inline identifiers::Id* IntTypeId() {
    static identifiers::Id* ptr = nullptr;
    if (ptr == nullptr) {
        ptr = identifiers::GetId("int");
    }
    return ptr;
}

class IntType : public ast::Type { //TODO transform to singleton?
  public:
    IntType(identifiers::Id* id) : Type(id) {}

    inline std::string llvm_name() override {
        return "i32";
    }

    inline int def_alignment() override {
        return 4;
    }
};

class IntTypeBinPlus : public ast::Fun {
  public:
    IntTypeBinPlus() : Fun(identifiers::GetId("operator+"),
                           ast::RType(IntTypeId()),
                           { 
                               new ast::Var(identifiers::GetId(".lhs"),
                               ast::RType(IntTypeId())),
                               new ast::Var(identifiers::GetId(".rhs"),
                               ast::RType(IntTypeId()))
                           },
                           {},
                           {}) {  }
    
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

inline identifiers::Id* StrTypeId() {
    static identifiers::Id* ptr = nullptr;
    if (ptr == nullptr) {
        ptr = identifiers::GetId("str");
    }
    return ptr;
}

class StrType : public ast::Type {
  public:
    StrType(identifiers::Id* id) : Type(id) {}

    inline std::string llvm_name() override {
        return "i8*";
    }

    inline int def_alignment() override {
        return 8;
    }
};

class StrTypeBinPlus : public ast::Fun {
  public:
    StrTypeBinPlus() : Fun(identifiers::GetId("operator+"),
                           ast::RType(StrTypeId()),
                           { 
                               new ast::Var(identifiers::GetId(".lhs"),
                               ast::RType(StrTypeId())),
                               new ast::Var(identifiers::GetId(".rhs"),
                               ast::RType(StrTypeId()))
                           },
                           {},
                           {}) {  }
    
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

void RegisterTypes();

} // namespace builtin

} // namespace compiler

#endif // TYPES_HPP

