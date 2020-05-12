#include "log.hpp"

namespace compiler {

//IMPROVEMENT Initializing with std iostreams may cause an initialization fiasco.
// It would be wise to move the reference of the log to a late initialization.
// Maybe, a program class could be created that managed initialization instead
// of using global variables.
BasicLogger lexical_log(std::cerr, "Lexical Error: ");
BasicLogger syntactic_log(std::cerr, "Syntax Error: ");
BasicLogger semantic_log(std::cerr, "Semantic Error: ");
BasicLogger warning_log(std::cerr, "Warning: ");
BasicLogger internal_log(std::cerr, "Internal Error: ");

} // namespace compiler
