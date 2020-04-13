#ifndef IDENTIFIERS_HPP
#define IDENTIFIERS_HPP

#include <iostream>
#include <cstring>
#include <string>
#include <vector>
#include <unordered_map>
#include <assert.h>
#include "function.hpp"
#include "type.hpp"
#include "expression.hpp"

namespace AST {

// Needed because of circular dependencies
class t_int_lit;
class t_function;
class t_id;

/**
 * @brief Intrinsec data to a variable
 */
class t_var { //TODO considerar poner como struct
  public:
    t_var(std::string& name, t_id* type) : name_(name), type_(type) {  }

    inline t_id* type() {
        return type_;
    }

  private:
    std::string& name_;
    t_id* type_;
};

/**
 * @brief Intrinsec data to a constant
 */
class t_constant {
  public:
    t_constant(std::string& name, t_int_lit* val) : name_(name), val_(val) {  }
    
    inline t_id* type() {
        return type_;
    }

  private:
    std::string& name_;
    t_id* type_;
    t_int_lit* val_;
};

class t_constants : public std::vector<t_constant*> {

};

/**
 * @brief An identifier.
 * 
 * A name that represents a type/variable/constant/function(s).

 * It is important to differentiate between t_id and t_var (or t_type or
 * t_function) and variable. The identifier t_id is just the name (with
 * the data associated), while the variable t_var is represents the use
 * of a variable, as lvalue or rvalue. //TODO
 */
class t_id : public t_expression {
  public:
    /**
     * @brief Gets the t_id object associated with a name or creates one.
     */
    static t_id* named(const std::string& name);

    inline const std::string& name() {
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
    bool register_function(t_function* func);

    /**
     * @brief Associates a var with this id.
     * 
     * An error is detected if the id is associated to another kind of
     * object or there was already a var with this name.
     * 
     * @returns if the var was registered.
     */
    bool register_as_variable(t_id* type);

    /**
     * @brief Associates a constant with this id.
     * 
     * An error is detected if the id is associated to another kind of
     * object or there was already a constant with this name.
     * 
     * @returns if the constant was registered.
     */
    bool register_as_constant(t_int_lit* val);

    /**
     * @brief Associates a type with this id.
     * 
     * An error is detected if the id is associated to another kind of
     * object or there was already a type with this name.
     * 
     * @returns if the type was registered.
     */
    bool register_as_type(t_type* type);

    /**
     * @returns the function associated with this identifier or nullptr if there
     * isn't such a function.
     */
    t_function* can_be_called(const std::vector<t_id*>& signature);

    inline bool is_a_variable() {
        return obj_type_ == VARIABLE;
    } //TODO considerar hacer estas 3 funciones devolver un puntero

    inline bool is_a_constant() {
        return obj_type_ == CONSTANT;
    }

    inline bool is_a_type() {
        return obj_type_ == TYPE;
    }

    /** Methods when behaving as a expression **/
    
    /**
     * @brief //TODO when behaving as lvalue
     */
    inline std::string llvm_var_name(std::ostream& os, int& local_var_count) {
        assert(obj_type_ == VARIABLE);
        return "%" + name_;
    }

    virtual t_id* exp_type();

    /**
     * @brief //TODO rvalue
     */
    virtual std::string llvm_eval(std::ostream& os, int& local_var_count);

    /** Methods when behaving as an specific obj **/ //TODO create a expression that references id and has things and the above things?

    void llvm_var_alloca(std::ostream& os);

    inline const std::string& llvm_type_name() {
        assert(obj_type_ == TYPE);
        return obj_data_.type->llvm_name();
    }

    void print(int lvl); //TODO marcar que deja de ser virtual

  private:
    std::string name_;

    enum ObjType {
        UNDECLARED,
        TYPE,
        VARIABLE,
        CONSTANT,
        FUNCTION
    };
    ObjType obj_type_;

    class ObjData {
      public:
        std::vector<t_function*> funcs;
        t_var* var;
        t_constant* cons;
        t_type* type;

        ObjData() {
            //TODO std::memset(this, 0, sizeof(ObjData));
        }

        ~ObjData() {

        }
    } obj_data_;

    static std::vector<t_id*> program_identifiers;
    static std::unordered_map<std::string, t_id*>* identifiers_look_up;

    t_id(const std::string& name);
};

}  // namespace

#endif // IDENTIFIERS_HPP

