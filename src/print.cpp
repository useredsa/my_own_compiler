/*#include "ast.hpp"
#include "errors.hpp"
#include "statements.hpp"
#include "expressions.hpp"

namespace compiler {

class AstPrinter {
  public:
    AstPrinter(std::ostream& os) : log(os) {  }

    template<typename T>
    inline void PrintIndented(T obj) {
        log.AddPrefix("    ");
        (*this)(obj);
        log.PopPrefix("    ");
    }

    // template<typename T>
    // inline void PrintIndented(T* ptr) {
    //     log.AddPrefix("    ");
    //     this(ptr);
    //     log.PopPrefix("    ");
    // }

    template<typename T>
    inline void PrintIndented(const std::vector<T>& vec) {
        log.AddPrefix("    ");
        for (auto obj : vec)
            (*this)(obj);
        log.PopPrefix("    ");
    }

    void operator()(const ast::Stmt& stmt) {
        std::visit(*this, stmt);
    }

    void operator()(const ast::Exp& exp) {
        std::visit(*this, exp);
    }

    void operator()(const ast::Program* prog) {
        log << "Prog\n";
        log << "--Function Decls:\n";
        PrintIndented(prog->funcs);
        log << "--Main:\n";
        PrintIndented(&prog->main);
        log << "\n";
    }

    void operator()(const ast::Dcls* dcls) {
        log << "Dcls:\n";
        log << "--Constants\n";
        PrintIndented(dcls->constants);
        log << "--Variables\n";
        PrintIndented(dcls->variables);
    }

    void operator()(const ast::Function* func) {
        log << "Function " << func->name() << "\n";
        log << "--Return Type:\n";
        PrintIndented(func->type());
        // for (auto arg : args_) {
        //     arg->print(lvl+2);
        // } //TODO
        log << "--signature\n";
        PrintIndented(func->signature());
        log << "--declarations\n";
        PrintIndented(func->dcls());
        log << "--statements:\n";
        PrintIndented(func->stmts());
    }

    void operator()(const ast::EmptyStmt* empty) {
        log << "EmptyStmt\n";
    }

    void operator()(const ast::Assig* assig) {
        log << "Assig\n";
        PrintIndented(assig->id);
        PrintIndented(assig->exp);
    }

    void operator()(const ast::IfStmt* if_stmt) {
        log << "IfStmt\n";
        PrintIndented(if_stmt->exp);
        log << "--then\n";
        PrintIndented(if_stmt->stmt);
        log << "--else\n";
        PrintIndented(if_stmt->alt_stmt);
    }

    void operator()(const ast::WhileStmt* while_stmt) {
        log << "WhileStmt\n";
        PrintIndented(while_stmt->exp);
        log << "--do\n";
        PrintIndented(while_stmt->stmt);
    }

    void operator()(const ast::ForStmt* for_stmt) {
        log << "ForStmt\n";
        PrintIndented(for_stmt->id);
        log << "-- :=\n";
        PrintIndented(for_stmt->start_exp);
        log << "--to\n";
        PrintIndented(for_stmt->end_exp);
        log << "--do\n";
        PrintIndented(for_stmt->stmt);
    }

    void operator()(const ast::ReadStmt* read_stmt) {
        log << "ReadStmt\n";
        PrintIndented(read_stmt->ids);
    }

    void operator()(const ast::WriteStmt* write_stmt) {
        log << "WriteStmt\n";
        PrintIndented(write_stmt->exps);
    }

    void operator()(const ast::IntLit* int_lit) {
        log << "IntLit" << int_lit->lit << "\n";
    }

    void operator()(const ast::StrLit* str_lit) {
        log << "StrLit" << str_lit->lit << "\n";
    }

    void operator()(const ast::FuncCall* call) {
        log << "FuncCall to " << call->id->name() << "\n";
        PrintIndented(call->args);
    }

    void operator()(const ast::Id* id) {
        log << "Id " << id->name() << "\n";
    }

    template<ast::UnaryOperators op>
    void operator()(const ast::UnaOp<op>& una_op) {
        log << "UnaOp<" << (char) op << ">\n";
        PrintIndented(una_op->exp);
    }

    template<ast::BinaryOperators op>
    void operator()(const ast::BinOp<op>& bin_op) {
        log << "BinOp<" << (char) op << ">\n";
        PrintIndented(bin_op->lhs);
        log << (char) op << "\n";
        PrintIndented(bin_op->rhs);
    }


  private:
    class TreeLogger : public Logger {
      public:
        TreeLogger(std::ostream& os) : Logger(os), prefix_() {  }

        void AddPrefix(const std::string& s) {
            prefix_ += s;
        }

        void PopPrefix(const std::string& s) {
            assert(s.size() < prefix_.size());
            // assert(s == prefix_.substr(prefix_.size()-s.size(), s.size()));
            prefix_.resize(prefix_.size()-s.size());
        }

      protected:
        inline virtual std::string prefix() { return prefix_; }

      private:
        std::string prefix_;
    } log;
};

}  // namespace compiler
*/
