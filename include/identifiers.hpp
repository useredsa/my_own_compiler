#ifndef IDENTIFIERS_HPP
#define IDENTIFIERS_HPP

#include <iostream>
#include <string>
#include <unordered_map>
#include "expressions.hpp"

namespace AST {


// Names dictionary
extern std::unordered_map<std::string, int> id_lookup;

struct id_info {
    std::string name;
    std::string llvm_id = "UNSET";

    id_info(std::string name) : name(name) {  }
};

extern std::vector<id_info> id_data;

int id_register(std::string& lexem);

class t_id : public t_expression {
  private:
    int id;

  public:
    t_id(int id) : id(id) {  }

    const std::string& llvm_ref(std::ostream& os, int& local_var_count) {
        std::string& llvm_id = id_data[id].llvm_id;
        if (llvm_id == "UNSET") {
            llvm_id = "%" + std::to_string(local_var_count);
            ++local_var_count;
            os << "\t" << llvm_id << " = alloca i32, align 4\n";
            llvm_id = "i32* " + llvm_id;
        }
        return llvm_id;
    }
    
    std::string llvm_eval(std::ostream& os, int& local_var_count) {
        std::string ref = "%" + std::to_string(local_var_count);
        ++local_var_count;
        std::string addr_ref = llvm_ref(os, local_var_count);
        os << "\t" << ref << " = " << "load i32, " << addr_ref << ", align 4\n";
        return "i32 " + ref;
    }

    int val() {
        int tmp = rand()%10;
        std::cerr << "assigned id " << id << " random value " << tmp << std::endl;
        return tmp;
    }

    void print(int lvl) {
        std::cout << std::string(lvl, '\t') << "id: " << id << ": " /*<< id_data[id]*/ << '\n'; //TODO
    }
};

using t_identifiers = std::vector<t_id*>;
// class t_identifiers {
//    
// };

}

#endif // IDENTIFIERS_HPP

