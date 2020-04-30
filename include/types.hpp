#ifndef TYPES_HPP
#define TYPES_HPP

#include <iostream>
#include <string>
#include "type.hpp"
#include "identifiers.hpp"
#include "namespace.hpp"

namespace compiler {

namespace ast {

namespace builtin {

inline Id* IntTypeId() {
    static Id* ptr = nullptr;
    if (ptr == nullptr)
        ptr = identifiers::GetId("integer");
    return ptr;
}

class IntType : public Type { //TODO transform to singleton?
  public:
    inline const std::string& llvm_name() {
        return llvm_name_;
    }

  private:
    static const std::string llvm_name_;
};

inline Id* StrTypeId() {
    static Id* ptr = nullptr;
    if (ptr == nullptr)
        ptr = identifiers::GetId("str");
    return ptr;
}

class StrType : public Type {
  public:
    inline const std::string& llvm_name() {
        return llvm_name_;
    }

  private:
    static const std::string llvm_name_;
};

void RegisterTypes();

} // namespace builtin

} // namespace ast

} // namespace compiler

#endif // TYPES_HPP

