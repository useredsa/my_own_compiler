#include "identifiers.hpp"

namespace AST {

std::unordered_map<std::string, int> id_lookup;

std::vector<id_info> id_data;

int id_register(std::string& lexem) {
    id_data.emplace_back(lexem);
    return id_data.size()-1;
}

}
