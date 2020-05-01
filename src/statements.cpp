#include "statements.hpp"

#include "errors.hpp"

namespace compiler {

namespace ast {

void Assig::llvm_put(std::ostream& os, int& local_var_count) {
    std::string value = exp->llvm_eval(os, local_var_count);
    std::string addr = id->llvm_var_name(os, local_var_count);
    os << "\tstore " << exp->exp_type()->llvm_type_name() << " " << value << ", "
       << id->exp_type()->llvm_type_name() << "* " << addr << ", align 4\n";
    //TODO quitar los align 4 de default por todo el código y poner lo que haya que poner
}

void Assig::print(int lvl) {
    std::cout << std::string(lvl, '\t') << "assignment\n";
    id->print(lvl+1);
    exp->print(lvl+1);
}

void IfStmt::llvm_put(std::ostream& os, int& local_var_count) {
    if (exp->exp_type() == builtin::IntTypeId()) {
        int label_num = local_var_count;  //TODO Llevar cuenta de las etiquetas? 
        std::string cond_value = exp->llvm_eval(os, local_var_count);
        os << "\t%" << local_var_count << " = icmp ne i32 " << cond_value << ", 0\n";
        os << "\tbr i1 %" << local_var_count++
           << ", label %then" << label_num
           << ", label %" << (alt_stmt != nullptr? "else" : "fi") << label_num << "\n";
        os << "then" << label_num << ":\n";
        stmt->llvm_put(os, local_var_count);
        if (alt_stmt != nullptr) {
            os << "\tbr label %fi" << label_num << "\n";  //TODO Por lo visto las etiquetas no se pueden mezclar con el code :(
            os << "else" << label_num << ":\n";
            alt_stmt->llvm_put(os, local_var_count);
            os << "\tbr label %fi" << label_num << "\n";
        }
        os << "fi" << label_num << ":\n";
    } else {
        semantic_error << "Incompatible type in if condition: expected integer\n";
    }
}

void IfStmt::print(int lvl) {
    std::string tabs = std::string(lvl, '\t');
    std::cout << tabs << "if\n";
    exp->print(lvl+1);
    std::cout << tabs << "then\n";
    stmt->print(lvl+1);
    if (alt_stmt != nullptr) {
        std::cout << tabs << "else\n";
        alt_stmt->print(lvl+1);
    }
}

void WhileStmt::llvm_put(std::ostream& os, int& local_var_count) {
    int label_num = local_var_count;
    os << "\tbr label %whilecomp" << label_num << "\n";
    os << "whilecomp" << label_num << ":\n";
    if (exp->exp_type() == builtin::IntTypeId()) {
        std::string cond_value = exp->llvm_eval(os, local_var_count);
        os << "\t%" << local_var_count << " = icmp ne i32 " << cond_value << ", 0\n";
        os << "\tbr i1 %" << local_var_count++
           << ", label %whileloop" << label_num
           << ", label %afterwhile" << label_num << "\n";
        os << "whileloop" << label_num << ":\n";
        stmt->llvm_put(os, local_var_count);
        os << "\tbr label %whilecomp" << label_num << "\n";
        os << "afterwhile" << label_num << ":\n";
    } else {
        semantic_error << "Incompatible type in while condition: expected integer\n";
    }
    //TODO
}

void WhileStmt::print(int lvl) {
    std::string tabs = std::string(lvl, '\t');
    std::cout << tabs << "while\n";
    exp->print(lvl+1);
    std::cout << tabs << "\tdo\n";
    stmt->print(lvl+1);
}

ForStmt::ForStmt(Id* id, IExp* start_exp, IExp* end_exp, IStmt* stmt)
        : id(id), start_exp(start_exp), end_exp(end_exp), stmt(stmt) {
    id->RegisterAsVariable(builtin::IntTypeId());
}

void ForStmt::llvm_put(std::ostream& os, int& local_var_count) {
    if (start_exp->exp_type() != identifiers::GetId("integer") ||
        end_exp->exp_type() != builtin::IntTypeId()) {
        if (start_exp->exp_type() != builtin::IntTypeId())
            semantic_error << "Incompatible type in initial value: expected integer\n";
        if (end_exp->exp_type() != builtin::IntTypeId())
            semantic_error << "Incompatible type in final value: expected integer\n";
    }  //TODO Arreglar la cutrez y no ejecutar lo siguiente

    id->llvm_var_alloca(os);
    int label_num = local_var_count;

    // control_id = begin  //TODO Lástima no aprovechar el contenido de assignment
    std::string init_value = start_exp->llvm_eval(os, local_var_count);
    std::string ctrl_id_addr = id->llvm_var_name(os, local_var_count);
    os << "\tstore " << start_exp->exp_type()->llvm_type_name() << " " << init_value
       << ", " << id->exp_type()->llvm_type_name() << "* " << ctrl_id_addr
       << ", align 4\n";
    std::string final_value = end_exp->llvm_eval(os, local_var_count);
    os << "\tbr label %forcomp" << label_num << "\n";

    // Loop until control_id == end
    os << "forcomp" << label_num << ":\n";
    std::string ctrl_id_value = id->llvm_eval(os, local_var_count);
    os << "\t%" << local_var_count << " = icmp ne "
       << identifiers::GetId("integer")->llvm_type_name() << " " << ctrl_id_value
       << ", " << final_value << "\n";
    os << "\tbr i1 %" << local_var_count++
       << ", label %forloop" << label_num
       << ", label %afterfor" << label_num << "\n";
    // for body
    os << "forloop" << label_num << ":\n";
    stmt->llvm_put(os, local_var_count);

    // increment control_id  //TODO Quitar literales?
    os << "\t%" << local_var_count << " = add nsw i32 " << ctrl_id_value << ", 1\n";
    os << "\tstore i32 %" << local_var_count++ << ", i32* " << ctrl_id_addr;
    os << "\tbr label %forcomp" << label_num << "\n";
    os << "afterfor" << label_num << ":\n";
}

void ForStmt::print(int lvl) {
    std::string tabs = std::string(lvl, '\t');
    std::cout << tabs << "for\n";
    id->print(lvl+1);
    std::cout << tabs << "\t:=\n";
    start_exp->print(lvl+1);
    std::cout << tabs << "\tto\n";
    end_exp->print(lvl+1);
    std::cout << tabs << "\tdo\n";
    stmt->print(lvl+1);
}

void ReadStmt::llvm_put(std::ostream& os, int& local_var_count) {
    for (Id* id : *ids) {
        if (!id->IsAVariable()) {
            //TODO y otro error más
            continue;
        }
        if (id->exp_type() == builtin::IntTypeId()) {
            std::string ref = id->llvm_var_name(os, local_var_count);
            os << "\t%" << local_var_count++ << " = call i32 (i8*, ...) "
               << "@__isoc99_scanf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* "
               << "@.io.int, i32 0, i32 0), " << id->exp_type()->llvm_type_name()
               << "* " << ref << ")\n";
        } else if (id->exp_type() == identifiers::GetId("str")) {
            //TODO
        } else {
            //TODO imprimir error read no soporta cosas que no sean builtin
        }
    }
}

void ReadStmt::print(int lvl) {
    std::cout << std::string(lvl, '\t') << "read statement\n";
    for (auto id : *ids) {
        id->print(lvl+1);
    }
}

void WriteStmt::llvm_put(std::ostream& os, int& local_var_count) {
    for (auto exp : *exps) {
        if (exp->exp_type() == builtin::IntTypeId()) {
            std::string ref = exp->llvm_eval(os, local_var_count);
            os << "\t%" << local_var_count++ << " = call i32 (i8*, ...) @printf(i8* "
               << "getelementptr inbounds ([3 x i8], [3 x i8]* @.io.int, i32 0, "
               << "i32 0), " << exp->exp_type()->llvm_type_name() << " " << ref
               << ")\n";
        } else if (exp->exp_type() == identifiers::GetId("str")) {
            std::string ref = exp->llvm_eval(os, local_var_count);
            os << "\t%" << local_var_count++ << " = call i32 (i8*, ...) @printf(i8* "
               << "getelementptr inbounds ([3 x i8], [3 x i8]* @.io.str, i32 0, "
               << "i32 0), " << exp->exp_type()->llvm_type_name() << " " << ref
               << ")\n";
        } else {
            //TODO imprimir error write no soporta cosas que no sean builtin
        }
    }
}

void WriteStmt::print(int lvl) {
    std::cout << std::string(lvl, '\t') << "write statement\n";
    for (auto exp : *exps) {
        exp->print(lvl+1);
    }
}

} // namespace ast

} // namespace compiler

