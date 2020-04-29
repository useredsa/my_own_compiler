#include "statements.hpp"

#include "namespace.hpp"
#include "errors.hpp"

namespace AST {

void t_assignment::llvm_put(std::ostream& os, int& local_var_count) {
    std::string value = exp_->llvm_eval(os, local_var_count);
    std::string addr = id_->llvm_var_name(os, local_var_count);
    os << "\tstore " << exp_->exp_type()->llvm_type_name() << " " << value << ", "
       << id_->exp_type()->llvm_type_name() << "* " << addr << ", align 4\n";
    //TODO quitar los align 4 de default por todo el código y poner lo que haya que poner
}

void t_assignment::print(int lvl) {
    std::cout << std::string(lvl, '\t') << "assignment\n";
    id_->print(lvl+1);
    exp_->print(lvl+1);
}

void t_if_then_statement::llvm_put(std::ostream& os, int& local_var_count) {
    if (cond_->exp_type() == t_namespace::get_id("integer")) { //TODO cambiar por referencia
        int label_num = local_var_count;  //TODO Llevar cuenta de las etiquetas? 
        std::string cond_value = cond_->llvm_eval(os, local_var_count);
        os << "\t%" << local_var_count << " = icmp ne i32 " << cond_value << ", 0\n";
        os << "\tbr i1 %" << local_var_count++
           << ", label %then" << label_num
           << ", label %else" << label_num << "\n";
        os << "then" << label_num << ":\n";
        conseq_->llvm_put(os, local_var_count);
        os << "\tbr label %else" << label_num << "\n";  //TODO Por lo visto las etiquetas no se pueden mezclar con el code :(
        os << "else" << label_num << ":\n";
    } else {
        semantic_error << "Incompatible type in if condition: expected integer\n";
    }
}

void t_if_then_statement::print(int lvl) {
    std::string tabs = std::string(lvl, '\t');
    std::cout << tabs << "if\n";
    cond_->print(lvl+1);
    std::cout << tabs << "then\n";
    conseq_->print(lvl+1);
}

void t_if_then_else_statement::llvm_put(std::ostream& os, int& local_var_count) {
    //TODO Ver cómo reutilizar t_if_then_statement::llvm_put :(
}

void t_if_then_else_statement::print(int lvl) {
    std::string tabs = std::string(lvl, '\t');
    t_if_then_statement::print(lvl);
    std::cout << tabs << "else\n";
    alternative_->print(lvl+2);
}

void t_while_statement::llvm_put(std::ostream& os, int& local_var_count) {
    int label_num = local_var_count;
    os << "\tbr label %whilecomp" << label_num << "\n";
    os << "whilecomp" << label_num << ":\n";
    if (cond_->exp_type() == t_namespace::get_id("integer")) {
        std::string cond_value = cond_->llvm_eval(os, local_var_count);
        os << "\t%" << local_var_count << " = icmp ne i32 " << cond_value << ", 0\n";
        os << "\tbr i1 %" << local_var_count++
           << ", label %whileloop" << label_num
           << ", label %afterwhile" << label_num << "\n";
        os << "whileloop" << label_num << ":\n";
        statement_->llvm_put(os, local_var_count);
        os << "\tbr label %whilecomp" << label_num << "\n";
        os << "afterwhile" << label_num << ":\n";
    } else {
        semantic_error << "Incompatible type in while condition: expected integer\n";
    }
    //TODO
}

void t_while_statement::print(int lvl) {
    std::string tabs = std::string(lvl, '\t');
    std::cout << tabs << "while\n";
    cond_->print(lvl+1);
    std::cout << tabs << "\tdo\n";
    statement_->print(lvl+1);
}

t_for_statement::t_for_statement(t_id* ctrl_id, t_expression *begin,
                                 t_expression *end, t_statement *statement)
        : control_id_(ctrl_id), begin_(begin), end_(end), statement_(statement) {
    control_id_->register_as_variable(t_namespace::get_id("integer"));
}

void t_for_statement::llvm_put(std::ostream& os, int& local_var_count) {
    if (begin_->exp_type() != t_namespace::get_id("integer") ||
        end_->exp_type() != t_namespace::get_id("integer")) {
        if (begin_->exp_type() != t_namespace::get_id("integer"))
            semantic_error << "Incompatible type in initial value: expected integer\n";
        if (end_->exp_type() != t_namespace::get_id("integer"))
            semantic_error << "Incompatible type in final value: expected integer\n";
    }  //TODO Arreglar la cutrez y no ejecutar lo siguiente

    control_id_->llvm_var_alloca(os);
    int label_num = local_var_count;

    // control_id = begin  //TODO Lástima no aprovechar el contenido de assignment
    std::string init_value = begin_->llvm_eval(os, local_var_count);
    std::string ctrl_id_addr = control_id_->llvm_var_name(os, local_var_count);
    os << "\tstore " << begin_->exp_type()->llvm_type_name() << " " << init_value
       << ", " << control_id_->exp_type()->llvm_type_name() << "* " << ctrl_id_addr
       << ", align 4\n";
    std::string final_value = end_->llvm_eval(os, local_var_count);
    os << "\tbr label %forcomp" << label_num << "\n";

    // Loop until control_id == end
    os << "forcomp" << label_num << ":\n";
    std::string ctrl_id_value = control_id_->llvm_eval(os, local_var_count);
    os << "\t%" << local_var_count << " = icmp ne "
       << t_namespace::get_id("integer")->llvm_type_name() << " " << ctrl_id_value
       << ", " << final_value << "\n";
    os << "\tbr i1 %" << local_var_count++
       << ", label %forloop" << label_num
       << ", label %afterfor" << label_num << "\n";
    // for body
    os << "forloop" << label_num << ":\n";
    statement_->llvm_put(os, local_var_count);

    // increment control_id  //TODO Quitar literales?
    os << "\t%" << local_var_count << " = add nsw i32 " << ctrl_id_value << ", 1\n";
    os << "\tstore i32 %" << local_var_count++ << ", i32* " << ctrl_id_addr;
    os << "\tbr label %forcomp" << label_num << "\n";
    os << "afterfor" << label_num << ":\n";
}

void t_for_statement::print(int lvl) {
    std::string tabs = std::string(lvl, '\t');
    std::cout << tabs << "for\n";
    control_id_->print(lvl+1);
    std::cout << tabs << "\t:=\n";
    begin_->print(lvl+1);
    std::cout << tabs << "\tto\n";
    end_->print(lvl+1);
    std::cout << tabs << "\tdo\n";
    statement_->print(lvl+1);
}

void t_read::llvm_put(std::ostream& os, int& local_var_count) {
    for (t_id* id : *ids) {
        if (!id->is_a_variable()) {
            //TODO y otro error más
            continue;
        }
        if (id->exp_type() == t_namespace::get_id("integer")) {
            std::string ref = id->llvm_var_name(os, local_var_count);
            os << "\t%" << local_var_count++ << " = call i32 (i8*, ...) "
               << "@__isoc99_scanf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* "
               << "@.io.int, i32 0, i32 0), " << id->exp_type()->llvm_type_name()
               << "* " << ref << ")\n";
        } else if (id->exp_type() == t_namespace::get_id("str")) {
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
        if (exp->exp_type() == t_namespace::get_id("integer")) {
            std::string ref = exp->llvm_eval(os, local_var_count);
            os << "\t%" << local_var_count++ << " = call i32 (i8*, ...) @printf(i8* "
               << "getelementptr inbounds ([3 x i8], [3 x i8]* @.io.int, i32 0, "
               << "i32 0), " << exp->exp_type()->llvm_type_name() << " " << ref
               << ")\n";
        } else if (exp->exp_type() == t_namespace::get_id("str")) {
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
