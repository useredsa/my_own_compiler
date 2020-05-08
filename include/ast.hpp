#ifndef AST_HPP
#define AST_HPP

#include <vector>
#include <string>
#include "ast_defs.hpp"

namespace compiler {

namespace ast {

/**
 * @brief A type.
 * 
 * Declaration an implementation shall end up in this class.
 * Constructions means declaration, and therefore registration associated to the name.
 */
class Type {
  public:
    Type(identifiers::Id* id);

    inline identifiers::Id* id() const {
        return id_;
    }

    virtual std::string llvm_name() = 0;
    virtual int def_alignment() = 0;

  private:
    identifiers::Id* id_;
};

/**
 * @brief A variable.
 * 
 * Declaration an implementation shall end up in this class.
 * Constructions means declaration, and therefore registration associated to the name.
 */
class Var {
  public:
    Var(identifiers::Id* id, RType rtype);
    Var(identifiers::Id* id, RType rtype, Exp val);

    inline identifiers::Id* id() const {
        return id_;
    }

    inline const RType& rtype() const {
        return rtype_;
    }

    inline RType& rtype() {
        return rtype_;
    }

    inline Exp val() const {
        return val_;
    }

  private:
    identifiers::Id* id_;
    RType rtype_;
    Exp val_;
};

extern std::vector<Var*> program_vars;

/**
 * @brief A function.
 * 
 * Declaration an implementation shall end up in this class.
 * Constructions means declaration, and therefore registration associated to the name.
 */
class Fun {
  public:
    Fun(identifiers::Id* id,
        RType rtype,
        std::vector<Var*>&& args,
        std::vector<Var*>&& var_dcls,
        std::vector<Stmt>&& stmts);

    inline identifiers::Id* id() const {
        return id_;
    }

    inline RType rtype() const {
        return rtype_;
    }

    inline RType& rtype() {
        return rtype_;
    }

    inline const std::vector<Var*>& args() const {
        return args_;
    }

    inline const std::vector<Var*>& var_dcls() const {
        return var_dcls_;
    }

    inline const std::vector<Stmt>& stmts() const {
        return stmts_;
    }
    //TODO poner que las funciones inline sobreescriben esto para poner un inline
    // virtual std::string llvm_put_call(std::ostream& os,
    //                                   int& local_var_count,
    //                                   const std::vector<std::string*>& params);

    /**
     * @brief Adds a list of constants declarations
     */
    // void AddConstants(const std::vector<std::pair<identifiers::Id*, IntLit*>>&  cons);

    /**
     * @brief Adds a list of variables declarations? //TODO Por qué identifiers?
     */
    // void AddIdentifiers(const std::vector<identifiers::Id*>& ids, identifiers::Id* type);

  private:
    identifiers::Id* id_;
    RType rtype_;
    std::vector<Var*> args_;
    std::vector<Var*> var_dcls_;
    std::vector<Stmt> stmts_;
};

/**
 * @brief Root node of the AST
 */
struct Prog {
    std::string name;
    std::vector<Fun*> funs;

    Prog(std::string&& name,
         std::vector<Fun*>&& funs,
         std::vector<Var*>&& var_dcls,
         std::vector<Stmt>&& stmts);
};

}  // namespace ast

} // namespace compiler

#endif // AST_HPP

