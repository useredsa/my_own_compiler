#ifndef IDENTIFIERS_HPP
#define IDENTIFIERS_HPP

#include <assert.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <utility>
#include <stack>
#include "ast_defs.hpp"
#include "log.hpp"

namespace compiler {

namespace identifiers {

class NameResolution;



enum NameScopeType {
    kCronological,
    kAcronological
};

/**
 * @brief Name scope object.
 * 
 * Comparable by address.
 */
class NameScope {
  public:
    NameScope(NameScopeType type, NameScope* parent);

    inline NameScope* parent() const {
        return parent_;
    }

    inline NameScopeType type() const {
        return type_;
    }

    inline int depth() const {
        return depth_;
    }

  private:
    NameScope* parent_;
    NameScopeType type_;
    int depth_;

    friend NameResolution;
};

/**
 * @brief Language abstractions that can be declared or named
 */
enum NamedAbstractions {
    kUnresolved,
    kRedirected,
    kType,
    kVariable,
    kFunctions
};

/**
 * @brief An identifier.
 * 
 * A name that represents a named abstraction (see NamedAbstractions).
 *
 * It is important to differentiate between Id and Var (or Type or
 * Fun). The identifier (Id) is just the name (with the data associated
 * to the name itself, for scope resolution purposes), while the variable
 * (Var) hols the information of a variable.
 */
class Id {
  public:

    Id(NameScope* scope, std::string&& name)
        : scope_(scope), name_(name), abstracts_(kUnresolved), ref() {  }

    ~Id();
    
    inline NameScope* namescope() const {
        return scope_;
    }
    
    inline const std::string& name() const {
        return name_;
    }

    /**
     * @brief Associates a function with this id.
     * 
     * An error is detected if the id is associated to another kind of
     * object or there was already a function with this name and signature.
     * 
     * @returns if the function was registered.
     */
    bool RegisterFunction(ast::Fun* fun);

    /**
     * @brief Associates a var with this id.
     * 
     * An error is detected if the id is associated to another kind of
     * object or there was already a var with this name.
     * 
     * @returns if the var was registered.
     */
    bool RegisterAsVariable(ast::Var* var);

    /**
     * @brief Associates a type with this id.
     * 
     * An error is detected if the id is associated to another kind of
     * object or there was already a type with this name.
     * 
     * @returns if the type was registered.
     */
    bool RegisterAsType(ast::Type* type);

    inline bool IsAVariable() {
        return abstracts_ == kVariable;
    } //TODO considerar hacer estas 3 funciones devolver un puntero

    inline bool IsAType() {
        return abstracts_ == kType;
    }

    inline ast::Var* var() {
        if (abstracts_ == kVariable) {
            return ref.var;
        }
        if (abstracts_ == kRedirected) {
            return ref.id->ref.var;
        }
        internal_log << "Unexpected call to Id::var()!" << std::endl;
        exit(-1);
    }

    inline ast::Type* type() {
        if (abstracts_ == kType) {
            return ref.type;
        }
        if (abstracts_ == kRedirected) {
            return ref.id->ref.type;
        }
        internal_log << "Unexpected call to Id::type()!" << std::endl;
        exit(-1);
    }

  private:
    NameScope* scope_;
    std::string name_;
    NamedAbstractions abstracts_;
    union AbstractionReference {
        /**
         * The destruction of non-trivial members
         * is delegated to the class constructor
         */
        ast::Type* type;
        ast::Var* var;
        std::vector<ast::Fun*> funs;
        Id* id;

        AbstractionReference() {
            type = nullptr;
        }

        ~AbstractionReference() {  }
    } ref;

    friend NameResolution;
};

struct NameInfo {
    std::stack<std::pair<size_t, Id*>> active_declarations;
    std::vector<Id*> ids;
};

extern std::unordered_map<std::string, NameInfo> name_table;

NameScope* current_name_scope();

/**
 * @brief Creates a new name scope on top of the current one.
 */
NameScope* AddNameScope(NameScopeType type = kCronological);

/**
 * @brief Sets the parent of the current namescope as the current one.
 */
void AbandonCurrentNameScope();

/**
 * @brief Creates an Id for a new name in the current name scope.
 */
Id* NewId(std::string&& name);

/**
 * @brief Gets the Id associated with a name in the CURRENT name scope.
 * 
 * Name resolution techniques apply. The Id may be unresolved.
 */
Id* GetId(std::string&& name); //TODO error when calling with "agag"

} // namespace identifiers

} // namespace compiler

#endif // IDENTIFIERS_HPP

