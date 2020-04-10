#ifndef T_FUNCTION_HPP
#define T_FUNCTION_HPP

#include <iostream>
#include <vector>

#include "identifiers.hpp"
#include "types.hpp"
#include "statements.hpp"


namespace AST {

using t_constants = std::vector<t_assignment*>;
// class t_constants {
    
// };

class t_declarations {
  public:
    std::vector<t_assignment*> constants;
    std::vector<t_id*> variables;

    t_declarations() : constants(), variables() {  }

    void add_constants(t_constants *consts) {
        constants.insert(constants.end(), consts->begin(), consts->end());
    }

    void add_identifiers(t_identifiers *ids, t_type *type) {
        for (auto id : *ids) {
            //TODO register type in table
        }
        variables.insert(variables.end(), ids->begin(), ids->end());
    }

    void print(int lvl) {
        std::string tabs(lvl, '\t');
        std::cout << tabs << "declarations:\n";
        std::cout << tabs << "\tconstants\n";
        for (auto c : constants) {
            c->print(lvl+2);
        }
        std::cout << tabs << "\tvariables\n";
        for (auto v : variables) {
            v->print(lvl+2);
        }
    }
};

using t_arguments = t_expressions;
// class t_arguments {
    
// };

class t_function {
  private:
    t_id *id;
    t_identifiers *args;
    t_type *args_type, *return_type;
    t_declarations *declarations;
    t_statements *statements;

  public:
    t_function(t_id *id, t_identifiers *args, t_type *args_type,
               t_type *r_type, t_declarations *decls,
               t_statements *statements)
        : id(id), args(args), args_type(args_type), return_type(r_type),
          declarations(decls), statements(statements) {  }

    void print(int lvl) {
        std::string tabs(lvl, '\t');
        std::cout << "function " /*<< id_data[id]*/ << '\n';
        std::cout << "\n\targuments:\n";
        for (auto arg : *args) {
            arg->print(lvl+2);
        }
        //cout << "arguments type:\n";
        args_type->print(lvl+1);
        //cout << "return type:\n";
        return_type->print(lvl+1);
        statements->print(lvl+1);
    }
};

class t_functions : public std::vector<t_function*> {  };

}

#endif // T_FUNCTION_HPP

