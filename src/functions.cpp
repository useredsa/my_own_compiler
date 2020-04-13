#include "functions.hpp"

namespace AST{
    
std::unordered_map<std::string, int> type_lookup;

std::vector<type_info> program_types;

int type_register(std::string& lexem) {
    program_types.emplace_back(lexem);
    return program_types.size()-1;
}

}

