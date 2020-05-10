#ifndef BUILTIN_HPP
#define BUILTIN_HPP

#include <ostream>
#include "ast.hpp"
#include "identifiers.hpp"

namespace compiler {

namespace builtin {

inline identifiers::Id* IntTypeId() {
    static identifiers::Id* ptr = nullptr;
    if (ptr == nullptr) {
        ptr = identifiers::GetId("int");
    }
    return ptr;
}

inline identifiers::Id* StrTypeId() {
    static identifiers::Id* ptr = nullptr;
    if (ptr == nullptr) {
        ptr = identifiers::GetId("str");
    }
    return ptr;
}

inline ast::Fun* ErrorFun() {
    static ast::Fun* ptr = nullptr;
    if (ptr == nullptr) {
        ptr = new ast::Fun(identifiers::NewId(".error_function"),
                           ast::RType(IntTypeId()),
                           {},
                           {},
                           {});
    }
    return ptr;
}

class ImaginaryErrorType : public ast::Type {
  public:
    ImaginaryErrorType(identifiers::Id* id) : Type(id) {}

    virtual inline std::string llvm_name() override {
        return ".error_type";
    }

    virtual inline int def_alignment() override {
        return 0;
    }
};

inline ast::Type* ErrorType() {
    static ast::Type* ptr = nullptr;
    if (ptr == nullptr) {
        ptr = new ImaginaryErrorType(identifiers::NewId(".error_type"));
    }
    return ptr;
}

/**
 * @brief Registers the names of built-in types, operators and functions
 */
void RegisterBuiltins();

} // namespace builtin

} // namespace compiler

#endif // BUILTIN_HPP

