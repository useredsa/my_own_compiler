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
 */
void RegisterBuiltins();

} // namespace builtin

} // namespace compiler

#endif // BUILTIN_HPP

