#include "log.hpp"

namespace compiler {

SemanticErrorLogger semantic_log(std::cerr);

LexicalErrorLogger lexical_log(std::cerr);

WarningsLogger warning_log(std::cerr);

} // namespace compiler
