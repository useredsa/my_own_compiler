#ifndef BUILTIN_HPP
#define BUILTIN_HPP

#include <ostream>
#include "ast.hpp"
#include "identifiers.hpp"
#include "types.hpp"

namespace compiler {

namespace builtin {

/**
 * @brief Registers the names of built-in types, operators and functions
 * 
 * //TODO
 */
void RegisterBuiltins();

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

// inline ast::Type* ErrorType() {
//     static ast::Type* ptr = nullptr;
//     if (ptr == nullptr) {
//         ptr = new ast::Type(identifiers::NewId(".error_type"));
//     }
//     return ptr;
// }

} // namespace builtin

} // namespace compiler

#endif // BUILTIN_HPP

