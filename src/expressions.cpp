#include "expressions.hpp"

#include "errors.hpp"
#include "namespace.hpp"

namespace AST {

t_int_lit::t_int_lit(int lit) : lit_(lit) {  }

t_id* t_int_lit::exp_type() {
    return t_namespace::get_id("integer");
}

std::string t_int_lit::llvm_eval(std::ostream& os, int& local_var_count) {
    return std::to_string(lit_);
}

void t_int_lit::print(int lvl) {
    std::cout << std::string(lvl, '\t') << "int_lit: " << lit_ << '\n';
}


t_str_lit::t_str_lit(std::string *lit) : lit_(lit) {  }

t_id* t_str_lit::exp_type() {
    return t_namespace::get_id("str");
}

std::string t_str_lit::llvm_eval(std::ostream& os, int& local_var_count) {
    return "getelementptr inbounds ([4 x i8], [4 x i8]* " + llvm_id_
            + ", i32 0, i32 0)";
}

void t_str_lit::print(int lvl) {
    std::cout << std::string(lvl, '\t') << "str_lit: " << *lit_ << '\n';
}

// t_function_call::t_function_call(t_name* object, t_expressions* args)
//     : object(object), args(args) {  } //TODO ya está en el hpp

t_id* t_function_call::exp_type() {
    // return name->; //TODO construir la signatura con los argumentos y devolver el tipo de la función
    return name_->exp_type(); //TODO esto es para variables no funciones
}

std::string t_function_call::llvm_eval(std::ostream& os, int& local_var_count) {
    //TODO
    return "TODO";
}

void t_function_call::print(int lvl) {
    std::cout << std::string(lvl, '\t') << "call to " << "TODO\n";
    for (auto arg : *args_) {
        args_->print(lvl+1);
    }
}

t_unary_op::t_unary_op(const valid_op op, t_expression *exp)
    : op_(op), exp_(exp) {  }

t_id* t_unary_op::exp_type() {
    return t_namespace::get_id("integer"); //TODO
}

std::string t_unary_op::llvm_eval(std::ostream& os, int& local_var_count) {
    //TODO
    return "TODO";
}

void t_unary_op::print(int lvl) {
    std::string tabs(lvl, '\t');
    std::cout << tabs << "t_unary_op:\n";
    std::cout << tabs << "\t-\n";
    exp_->print(lvl+1);
    std::cout << '\n';
}

std::string t_binary_op::op_string(valid_op op) {
    switch (op) {
        case plus:
        return "+";
        break;
        case minus:
        return "-";
        break;
        case asterisk:
        return "*";
        break;
        case slash:
        return "/";
        break;
        default:
        return "UNRECOGNIZED OP";
    }
}

t_binary_op::t_binary_op(const valid_op op, t_expression *lhs, t_expression *rhs) :
    op_(op), l_(lhs), r_(rhs) {    }

t_id* t_binary_op::exp_type() {
    return t_namespace::get_id("integer"); //TODO
}

std::string t_binary_op::llvm_eval(std::ostream& os, int& local_var_count) {
    t_function* func = t_namespace::get_id("operator+")->
                         can_be_called({l_->exp_type(), r_->exp_type()});
    if (func == nullptr) {
        //TODO otro error
        semantic_error << "no existe el operador!\n";
        return "{UNAVAILABLE OPERATOR}";
    }
    std::string lhs = l_->llvm_eval(os, local_var_count);
    std::string rhs = r_->llvm_eval(os, local_var_count);
    return func->llvm_put_call(os, local_var_count, {&lhs, &rhs});
}

void t_binary_op::print(int lvl) {
    std::string tabs(lvl, '\t');
    std::cout << tabs << "t_binary_op:\n";
    l_->print(lvl+1);
    std::cout << tabs << '\t' << op_string(op_) << '\n';
    r_->print(lvl+1);
    std::cout << '\n';
}

} // namespace AST

