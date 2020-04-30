#ifndef FUNCTION_HPP
#define FUNCTION_HPP

#include <iostream>
#include <vector>
#include "identifiers.hpp"
#include "type.hpp"
#include "statement.hpp"

namespace compiler {
    
namespace ast {

class Id;
struct IntLit;

/**
 * @brief A list of declarations of constants and/or variables
 * 
 * //TODO
 */
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

    //TODO
    void llvm_put_constants(std::ostream& os);

    //TODO
    void llvm_put_variables(std::ostream& os);

    void print(int lvl);
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
             Stmts* statements);

    inline const std::vector<Id*>& signature() const { //TODO buscar otros getters y hacerlos const
        return signature_;
    }

    inline const std::string& name() {
        return name_;
    }

    inline Id* type() {
        return type_;
    }

    void llvm_put(std::ostream& os);

    //TODO poner que las funciones inline sobreescriben esto para poner un inline
    virtual std::string llvm_put_call(std::ostream& os,
                                      int& local_var_count,
                                      std::vector<std::string*> params);

    void print(int lvl);

  private:
    Id* type_;
    const std::string& name_;
    std::vector<Id*> args_;
    std::vector<Id*> signature_;
    Dcls* dcls_;
    Stmts* statements_;
};

class Functions : public std::vector<Function*> {

};

} // namespace ast

} // namespace compiler

#endif // FUNCTION_HPP

