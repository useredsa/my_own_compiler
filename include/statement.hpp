#ifndef STATEMENT_HPP
#define STATEMENT_HPP

#include <string>
#include <vector>

namespace compiler {

namespace ast {

class IStmt {
  public:
    virtual void print(int lvl) = 0;

    virtual void llvm_put(std::ostream& os, int& local_var_count) = 0;
};

class Stmts : public std::vector<IStmt*>, public IStmt {
  public:
    void llvm_put(std::ostream& os, int& local_var_count);

    void print(int lvl);
};

}  // namespace ast

} // namespace compiler

#endif // STATEMENT_HPP

