#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP

#include <iostream>
#include <vector>

#include <identifiers.hpp>
#include <types.hpp>
#include <statements.hpp>


namespace AST {

class t_function {
  private:
    t_id *id;
    t_identifiers *args;
    t_type *args_type, *return_type;
    t_declarations *declarations;
    t_compound_statement *statements;

  public:
    t_function(t_id *id, t_identifiers *args, t_type *args_type,
               t_type *r_type, t_declarations *decls,
               t_compound_statement *statements)
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

using t_functions = std::vector<t_function*>;
// class t_functions {
    
// };

}

#endif // FUNCTIONS_HPP

