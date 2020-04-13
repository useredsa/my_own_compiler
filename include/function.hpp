#ifndef FUNCTION_HPP
#define FUNCTION_HPP

#include <iostream>
#include <vector>
#include "identifiers.hpp"
#include "type.hpp"
#include "statement.hpp"

namespace AST {

// Because of circular dependencies, some declarations are needed.
class t_id;
class t_int_lit;

class t_declarations {
  public:
    std::vector<t_id*> constants;
    std::vector<t_id*> variables;

    t_declarations() : constants(), variables() {  }

    void add_constants(const std::vector<std::pair<t_id*, t_int_lit*>>&  cons);

    void add_identifiers(const std::vector<t_id*>& ids, t_id* type);

    void print(int lvl);
};

/**
 * @brief A function declaration.
 * 
 * A class that represents the declaration and implementation of a function.
 */
class t_function {
  public:
    t_function(t_id* type,
               t_id* name,
               const std::vector<std::pair<t_id*, t_id*>>& args,
               t_declarations* decls,
               t_statements* statements);

    inline const std::vector<t_id*>& signature() const { //TODO buscar otros getters y hacerlos const
        return signature_;
    }

    void print(int lvl);

  private:
    t_id* type_;
    std::vector<t_id*> args_;
    std::vector<t_id*> signature_;
    t_declarations* declarations_;
    t_statements* statements_;
};

class t_functions : public std::vector<t_function*> {

};

}

#endif // FUNCTION_HPP

