#ifndef BUILTIN_HPP
#define BUILTIN_HPP

#include <ostream>

namespace compiler {

namespace builtin {

/**
 * @brief Registers the names of built-in types, operators and functions
 * 
 * //TODO
 */
void RegisterBuiltins();

/**
 * @brief Outputs as global variables the program's str literals
 */
void LlvmPutStrLits(std::ostream& os);

} // namespace builtin

} // namespace compiler

#endif // BUILTIN_HPP

