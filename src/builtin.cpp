#include "builtin.hpp"

namespace AST {

t_int_lit::t_int_lit(int lit) : lit(lit) {  }

t_type* t_int_lit::exp_type() {
    return new builtin::t_int();
}

std::string t_int_lit::llvm_eval(std::ostream& os, int& local_var_count) {
    return std::to_string(lit);
}

void t_int_lit::print(int lvl) {
    std::cout << std::string(lvl, '\t') << "int_lit: " << lit << '\n';
}


t_str_lit::t_str_lit(std::string *lit) : lit(lit) {  }

t_type* t_str_lit::exp_type() {
    return new builtin::t_str();
}

std::string t_str_lit::llvm_eval(std::ostream& os, int& local_var_count) {
    return "getelementptr inbounds ([4 x i8], [4 x i8]* " + llvm_id
            + ", i32 0, i32 0)";
}

void t_str_lit::print(int lvl) {
    std::cout << std::string(lvl, '\t') << "str_lit: " << *lit << '\n';
}

t_function_call::t_function_call(t_name* object, t_expressions* args)
    : object(object), args(args) {  }

t_type* exp_type() {
    //TODO
    return new builtin::t_int();
}

std::string t_function_call::llvm_eval(std::ostream& os, int& local_var_count) {
    //TODO
    return "TODO";
}

void t_function_call::print(int lvl) {
    std::cout << std::string(lvl, '\t') << "call to " << "TODO\n";
    for (auto arg : *args) {
        args->print(lvl+1);
    }
}

t_unary_op::t_unary_op(const valid_op op, t_expression *exp)
    : op(op), exp(exp) {  }

t_type* t_unary_op::exp_type() {
    return new builtin::t_int();
}

// virtual std::string t_unary_op::llvm_eval(std::ostream& os, int& local_var_count) {
//     //TODO
//     return "TODO";
// }

void t_unary_op::print(int lvl) {
    std::string tabs(lvl, '\t');
    std::cout << tabs << "t_unary_op:\n";
    std::cout << tabs << "\t-\n";
    exp->print(lvl+1);
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
    op(op), l(lhs), r(rhs) {    }

t_type* t_binary_op::exp_type() {
    return new builtin::t_int();
}

std::string t_binary_op::llvm_eval(std::ostream& os, int& local_var_count) {
    std::string lhs = l->llvm_eval(os, local_var_count);
    std::string rhs = r->llvm_eval(os, local_var_count);
    std::string result = "%" + std::to_string(local_var_count++);
    os << "\t" << result << " = add nsw " << l->exp_type()->llvm_name()
        << " " << lhs << ", " << rhs << "\n";
    return result;
}

void t_binary_op::print(int lvl) {
    std::string tabs(lvl, '\t');
    std::cout << tabs << "t_binary_op:\n";
    l->print(lvl+1);
    std::cout << tabs << '\t' << op_string(op) << '\n';
    r->print(lvl+1);
    std::cout << '\n';
}

}  // namespace