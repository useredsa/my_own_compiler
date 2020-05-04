#ifndef TYPES_HPP
#define TYPES_HPP

#include <iostream>
#include <string>
#include "ast_defs.hpp"
#include "ast.hpp"
#include "identifiers.hpp"

namespace compiler {

namespace builtin {

inline identifiers::Id* IntTypeId() {
    static identifiers::Id* ptr = nullptr;
    if (ptr == nullptr) {
        ptr = identifiers::GetId("integer");
    }
    return ptr;
}

class IntType : public ast::Type { //TODO transform to singleton?
  public:
    inline std::string llvm_name() override {
        return "i32";
    }

    inline int def_alignment() override {
        return 4;
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
    inline std::string llvm_name() override {
        return "i8*";
    }

    inline int def_alignment() override {
        return 8;
    }
};

void RegisterTypes();

} // namespace builtin

} // namespace compiler

#endif // TYPES_HPP

