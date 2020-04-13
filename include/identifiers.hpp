#ifndef IDENTIFIERS_HPP
#define IDENTIFIERS_HPP

#include <iostream>
#include <cstring>
#include <string>
#include <vector>
#include <unordered_map>
#include "function.hpp"
#include "type.hpp"
#include "expression.hpp"

namespace AST {

/**
 * @brief Intrinsec data to a variable
 */
class t_var { //TODO considerar poner como struct
  public:
    t_var(t_type* type) : type_(type) {  }

    inline t_type* type() {
        return type_;
    }

  private:
    t_type* type_;
};

/**
 * @brief Intrinsec data to a constant
 */
class t_constant {
  public:
    t_constant(t_int_lit* val) : val_(val) {  }
    
    inline t_type* type() {
        return type_;
    }

  private:
    t_type* type_;
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
    bool register_as_variable(t_type* type);

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
     * @returns if there's a function with this identifier and signature.
     */ //TODO considerar hacer estas 3 funciones devolver un puntero
    bool can_be_called(const std::vector<t_id*>& signature);

    inline bool is_a_variable() {
        return obj_type_ == VARIABLE;
    }

    inline bool is_a_constant() {
        return obj_type_ == CONSTANT;
    }

    inline bool is_a_type() {
        return obj_type_ == TYPE;
    }


    /** Methods when behaving as a expression **/
    
    /**
     * @brief //TODO
     */
    // const std::string& llvm_ref(std::ostream& os, int& local_var_count) {
    //     // std::string& llvm_id = program_names[id].llvm_id;
    //     // if (llvm_id == "UNSET") {
    //     //     llvm_id = "%" + std::to_string(local_var_count);
    //     //     ++local_var_count;
    //     //     os << "\t" << llvm_id << " = alloca i32, align 4\n";
    //     //     llvm_id = llvm_id;
    //     // }
    //     return llvm_id;
    // }

    virtual t_type* exp_type();
    
    virtual std::string llvm_eval(std::ostream& os, int& local_var_count) {
        std::string ref = "%" + std::to_string(local_var_count);
        ++local_var_count;
        // std::string addr_ref = llvm_ref(os, local_var_count);
        // os << "\t" << ref << " = " << "load i32, " << exp_type()->llvm_name()
           // << "* " << addr_ref << ", align 4\n";
        return ref;
        //TODO
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

    union ObjData {
        std::vector<t_function*> funcs;
        t_var* var;
        t_constant* cons;
        t_type* type;

        ObjData() {
            std::memset(this, 0, sizeof(ObjData));
        }

        ~ObjData() {

        }
    } obj_data_;

    static std::vector<t_id*> program_identifiers;
    static std::unordered_map<std::string, t_id*> identifiers_look_up;

    t_id(const std::string& name);
};

}

#endif // IDENTIFIERS_HPP

