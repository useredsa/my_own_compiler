#ifndef TYPES_HPP
#define TYPES_HPP

#include <iostream>
#include <string>

#include "io.hpp"
#include "identifiers.hpp"
#include "expressions.hpp"

namespace AST {

class t_type : public t_id {  //TODO Emilio hippie
  public:
    t_type(int id) : t_id(id) {  };
};

class t_int_lit : public t_expression {
  public:
    const int lit;

    t_int_lit(int lit) : lit(lit) {  }

    std::string llvm_eval(std::ostream& os, int& local_var_count) {
        return "i32 " + std::to_string(lit);
    }

    void print(int lvl) {
        std::cout << std::string(lvl, '\t') << "int_lit: " << lit << '\n';
    }
};

class t_str_lit : public t_expression {
  private:
    std::string *lit;
    std::string llvm_id = "TODO";
    
  public:
    t_str_lit(std::string *lit) : lit(lit) {  }

    std::string llvm_eval(std::ostream& os, int& local_var_count) {
        return "i8* getelementptr inbounds ([4 x i8], [4 x i8]* " + llvm_id
               + ", i32 0, i32 0)";
    }

    void print(int lvl) {
        std::cout << std::string(lvl, '\t') << "str_lit: " << *lit << '\n';
    }
};

namespace builtin {
     static t_type integer(-1); //TODO see correct way of initialization

     // class t_write : public t_statement {
     //   public:
     //       t_print_list* print_list;

     //       t_write(t_print_list* print_list) : print_list(print_list) {  }

     //       void print(int lvl) {
     //           string tabs(lvl, '\t');
     //           cout << tabs << "print_list:\n";
     //           for ()
               
     //       }
     // }
};

}

#endif // TYPES_HPP

