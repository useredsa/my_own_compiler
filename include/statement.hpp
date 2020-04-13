#ifndef STATEMENT_HPP
#define STATEMENT_HPP

#include <iostream>
#include <string>
#include <vector>

namespace AST {

class t_statement {
  public:
    virtual void print(int lvl) = 0;

    virtual void llvm_put(std::ostream& os, int& local_var_count) = 0;
};

class t_statements : public std::vector<t_statement*>, public t_statement {
  public:
    void llvm_put(std::ostream& os, int& local_var_count) {
        //TODO
    }

    void print(int lvl) {
        std::string tabs(lvl, '\t');
        std::cout << tabs << "begin\n";
        for (auto st : *this) {
            st->print(lvl+1);
        }
        std::cout << tabs << "end\n";
    }
};

}

#endif // STATEMENT_HPP

