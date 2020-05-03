#ifndef IDENTIFIERS_HPP
#define IDENTIFIERS_HPP

#include <assert.h>
#include <string>
#include <vector>

namespace compiler {

namespace ast {

struct Function;
struct IntLit;
class Id;
class Type;

/**
 * @brief Intrinsec data to a variable
 */
struct Var {
    Id* id;
    Id* type;
    Var(Id* id, Id* type) : id(id), type(type) {  }
};

/**
 * @brief Intrinsec data to a constant
 */
struct Constant {
    Id* id;
    IntLit* val;

    Constant(Id* id, IntLit* val) : id(id), val(val) {  }
};

struct Constants : public std::vector<Constant*> {  }; //TODO remove or put in another file?

enum NameScopeType {
    kCronological,
    kAcronological
};

/**
 * @brief Name scope object.
 * 
 * Comparable by address.
 */
struct NameScope {
    NameScope* parent;
    NameScopeType type;
};

/**
 * @brief Language abstractions that can be declared or named
 */
enum NamedAbstractions {
    kUnresolved,
    kRedirected,
    kType,
    kVariable,
    kConstant,
    kFunctions
};

/**
 * @brief An identifier.
 * 
 * A name that represents a named abstraction (see NamedAbstractions).
 *
 * It is important to differentiate between Itd and Var (or Type or
 * Function). The identifier (Id) is just the name (with the data
 * associated to the name itself), while the variable (Var) hols the
 * information of a variable.
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
    bool RegisterFunction(ast::Function* func);

    /**
     * @brief Associates a var with this id.
     * 
     * An error is detected if the id is associated to another kind of
     * object or there was already a var with this name.
     * 
     * @returns if the var was registered.
     */
    bool RegisterAsVariable(Id* type);

    /**
     * @brief Associates a constant with this id.
     * 
     * An error is detected if the id is associated to another kind of
     * object or there was already a constant with this name.
     * 
     * @returns if the constant was registered.
     */
    bool RegisterAsConstant(IntLit* val);

    /**
     * @brief Associates a type with this id.
     * 
     * An error is detected if the id is associated to another kind of
     * object or there was already a type with this name.
     * 
     * @returns if the type was registered.
     */
    bool RegisterAsType(Type* type);

    /**
     * @returns the function associated with this identifier or nullptr if there
     * isn't such a function.
     */
    ast::Function* can_be_called(const std::vector<Id*>& signature);

    inline bool IsAVariable() {
        return abstracts_ == kVariable;
    } //TODO considerar hacer estas 3 funciones devolver un puntero

    inline bool IsAConstant() {
        return abstracts_ == kConstant;
    }

    inline bool IsAType() {
        return abstracts_ == kType;
    }

    /** Methods when behaving as a expression **/
    
    /**
     * @brief //TODO when behaving as lvalue
     */
    // inline std::string llvm_var_name(std::ostream& os, int& local_var_count) {
    //     assert(abstracts_ == kVariable);
    //     return "%" + name_;
    // }

    // virtual Id* exp_type();

    /**
     * @brief //TODO rvalue
     */
    // virtual std::string llvm_eval(std::ostream& os, int& local_var_count);

    /** Methods when behaving as an specific obj **/ //TODO create a expression that references id and has things and the above things?

    // void llvm_var_alloca(std::ostream& os);

    // inline const std::string& llvm_type_name() {
    //     assert(abstracts_ == kType);
    //     return ref.type->llvm_name();
    // }

  private:
    NameScope* scope_;
    std::string name_;
    NamedAbstractions abstracts_;
    union AbstractionReference {
        /**
         * The destruction of non-trivial members
         * is delegated to the class constructor
         */
        Type* type;
        Var* var;
        Constant* cons;
        std::vector<ast::Function*> funcs;

        AbstractionReference() {
            type = nullptr;
        }

        ~AbstractionReference() {  }
    } ref;
};

} // namespace ast



namespace identifiers {

ast::NameScope* current_name_scope();

/**
 * @brief Creates a new name scope on top of the current one.
 */
ast::NameScope* AddNameScope(ast::NameScopeType type = ast::kCronological);

/**
 * @brief Sets the parent of the current namescope as the current one.
 */
void AbandonCurrentNameScope();

/**
 * @brief Creates an Id for a new name in the current name scope.
 */
ast::Id* NewId(std::string&& name);

/**
 * @brief Gets the Id associated with a name in the CURRENT name scope.
 * 
 * Name resolution techniques apply. The Id may be unresolved.
 */
ast::Id* GetId(std::string&& name);

} // namespace identifiers

} // namespace compiler

#endif // IDENTIFIERS_HPP

