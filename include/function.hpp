#ifndef FUNCTION_HPP
#define FUNCTION_HPP

#include <iostream>
#include <vector>
#include "statements.hpp"

namespace compiler {
    
namespace ast {

class Id;
struct IntLit;

/**
 * @brief A list of declarations of constants and/or variables
 * 
 * //TODO
 */
//TODO esta clase la quiero destripar
struct Dcls {
    std::vector<Id*> constants;
    std::vector<Id*> variables;

    Dcls() : constants(), variables() {  }

    /**
     * @brief Adds a list of constants declarations
     */
    void AddConstants(const std::vector<std::pair<Id*, IntLit*>>&  cons);

    /**
     * @brief Adds a list of variables declarations? //TODO Por qué identifiers?
     */
    void AddIdentifiers(const std::vector<Id*>& ids, Id* type);
};

/**
 * @brief A function declaration.
 * 
 * A class that represents the declaration and implementation of a function.
 */
class Function {
  public:
    Function(Id* type,
             Id* name,
             const std::vector<std::pair<Id*, Id*>>& args,
             Dcls* dcls,
             std::vector<Stmt>&& statements);

    inline Id* type() const {
        return type_;
    }

    inline const std::string& name() const {
        return name_;
    }

    inline const std::vector<Id*>& signature() const {
        return signature_;
    }

    inline const Dcls* dcls() const {
        return dcls_;
    }

    inline const std::vector<Id*>& args() const {
        return args_;
    }

    inline const std::vector<Stmt>& stmts() const {
        return statements_;
    }
    //TODO poner que las funciones inline sobreescriben esto para poner un inline
    // virtual std::string llvm_put_call(std::ostream& os,
    //                                   int& local_var_count,
    //                                   const std::vector<std::string*>& params);
  private:
    const std::string& name_;
    Id* type_;
    Dcls* dcls_;
    std::vector<Id*> args_;
    std::vector<Id*> signature_;
    std::vector<Stmt> statements_;
};

} // namespace ast

} // namespace compiler

#endif // FUNCTION_HPP

