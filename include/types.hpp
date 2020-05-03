#ifndef TYPES_HPP
#define TYPES_HPP

#include <iostream>
#include <string>
#include "identifiers.hpp"

namespace compiler {

namespace ast {

class Type {
  public:
    Type() {  };

    virtual const std::string& llvm_name() = 0;

    void print(int lvl) {
        std::cout << std::string(lvl, '\t') << "type\n";
    }
};

} // namespace ast



namespace builtin {

inline ast::Id* IntTypeId() {
    static ast::Id* ptr = nullptr;
    if (ptr == nullptr)
        ptr = identifiers::GetId("integer");
    return ptr;
}

class IntType : public ast::Type { //TODO transform to singleton?
  public:
    inline const std::string& llvm_name() {
        return llvm_name_;
    }

  private:
    static const std::string llvm_name_;
};

inline ast::Id* StrTypeId() {
    static ast::Id* ptr = nullptr;
    if (ptr == nullptr)
        ptr = identifiers::GetId("str");
    return ptr;
}

class StrType : public ast::Type {
  public:
    inline const std::string& llvm_name() {
        return llvm_name_;
    }

  private:
    static const std::string llvm_name_;
};

void RegisterTypes();

} // namespace builtin

} // namespace compiler

#endif // TYPES_HPP

