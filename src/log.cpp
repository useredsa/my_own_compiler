#include "log.hpp"

namespace compiler {

BasicLogger internal_log(std::cerr, "Internal Error: ");
BasicLogger semantic_log(std::cerr, "Semantic Error: ");
BasicLogger lexical_log(std::cerr, "Lexical Error: ");
BasicLogger warning_log(std::cerr, "Warning: ");

} // namespace compiler
