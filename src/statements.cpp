#include "statements.hpp"

namespace AST {

void t_assignment::llvm_put(std::ostream& os, int& local_var_count) {
    std::string value = exp->llvm_eval(os, local_var_count);
    std::string addr = id->llvm_var_name(os, local_var_count);
    os << "\tstore " << exp->exp_type()->llvm_type_name() << " " << value << ", "
       << id->exp_type()->llvm_type_name() << "* " << addr << ", align 4\n";
    //TODO quitar los align 4 de default por todo el código y poner lo que haya que poner
}

void t_assignment::print(int lvl) {
    std::cout << std::string(lvl, '\t') << "assignment\n";
    id->print(lvl+1);
    exp->print(lvl+1);
}

void t_read::llvm_put(std::ostream& os, int& local_var_count) {
    for (t_id* id : *ids) {
        if (!id->is_a_variable()) {
            //TODO y otro error más
            continue;
        }
        if (id->exp_type() == t_id::named("integer")) {
            std::string ref = id->llvm_var_name(os, local_var_count);
            os << "\t%" << local_var_count++ << " = call i32 (i8*, ...) "
               << "@__isoc99_scanf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* "
               << "@.io.int, i32 0, i32 0), " << id->exp_type()->llvm_type_name()
               << "* " << ref << ")\n";
        } else if (id->exp_type() == t_id::named("str")) {
            //TODO
        } else {
            //TODO imprimir error read no soporta cosas que no sean builtin
        }
    }
}

void t_read::print(int lvl) {
    std::cout << std::string(lvl, '\t') << "read statement\n";
    for (auto id : *ids) {
        id->print(lvl+1);
    }
}

void t_write::llvm_put(std::ostream& os, int& local_var_count) {
    for (auto exp : *exps) {
        if (exp->exp_type() == t_id::named("integer")) {
            std::string ref = exp->llvm_eval(os, local_var_count);
            os << "\t%" << local_var_count++ << " = call i32 (i8*, ...) @printf(i8* "
               << "getelementptr inbounds ([3 x i8], [3 x i8]* @.io.int, i32 0, "
               << "i32 0), " << exp->exp_type()->llvm_type_name() << " " << ref
               << ")\n";
        } else if (exp->exp_type() == t_id::named("str")) {
            //TODO
        } else {
            //TODO imprimir error write no soporta cosas que no sean builtin
        }
    }
}

void t_write::print(int lvl) {
    std::cout << std::string(lvl, '\t') << "write statement\n";
    for (auto exp : *exps) {
        exp->print(lvl+1);
    }
}

} // namespace AST
