#ifndef AST_HPP
#define AST_HPP

#include <string>
#include <vector>
#include <iostream> //TODO remove


using std::string;
using std::cout;

class t_print_item {
  public:
    virtual std::string str_val() = 0;
};

using t_print_list = std::vector<t_print_item*>;
// class t_print_list {
//   private:
//     std::vector<t_print_item*> items;

//   public:
//     t_print_list(t_print_item* first) : items() {
//         items.push_back(first);
//     }

//     void push_back(t_print_item* item) {
//         items.push_back(item);
//     }
// };

class t_expression : public t_print_item {
  public:
    // virtual int reg() = 0;
    virtual int val() = 0;
    virtual void print(int lvl) = 0;

    std::string str_val() {
        return std::to_string(val());
    }
};

class t_expressions {
  private:
    std::vector<t_expression*> exps;

  public:
    t_expressions() : exps() {  }

    t_expressions(t_expression* first) : exps() {
        exps.push_back(first);
    }

    void push_back(t_expression* exp) {
        exps.push_back(exp);
    }

    void print(int lvl) {
        string tabs(lvl, '\t');
        for (auto exp : exps) {
            exp->print(lvl+1);
            cout << '\n';
        }
    }
};

class t_unary_op : public t_expression {
  public:
    enum valid_op {
        minus
    };

    const valid_op op;
    t_expression *exp;

    t_unary_op(const valid_op op, t_expression* exp)
      : op(op), exp(exp) {  }

    int val() {
        return -exp->val();
    }

    void print(int lvl) {
        cout << string(lvl, '\t') << "t_unary_op:\n";
        cout << string(lvl+1, '\t') << "-\n";
        exp->print(lvl+1);
        cout << '\n';
    }
};

class t_binary_op : public t_expression {
  public:
    enum valid_op {
        plus, minus, asterisk, slash
    };
    
    const valid_op op;
    t_expression *l, *r;
    
    t_binary_op(const valid_op op, t_expression* lhs, t_expression *rhs)
      : op(op), l(lhs), r(rhs) {  }

    int val() {
        switch (op) {
          case plus:
            return l->val()+r->val();
            break;
          case minus:
            return l->val()-r->val();
            break;
          case asterisk:
            return l->val()*r->val();
            break;
          case slash:
            return l->val()/r->val();
            break;
          default:
            return 0; //TODO
        }
    }

    void print(int lvl) {
        cout << string(lvl, '\t') << "t_binary_op:\n";
        l->print(lvl+1);
        cout << string(lvl+1, '\t');
        switch (op) {
          case plus:
            cout << '+';
            break;
          case minus:
            cout << '-';
            break;
          case asterisk:
            cout << '*';
            break;
          case slash:
            cout << '/';
            break;
        }
        cout << '\n';
        r->print(lvl+1);
        cout << '\n';
    }
};

class t_int_lit : public t_expression {
  public:
    const int lit;

    t_int_lit(int lit) : lit(lit) {  }

    int val() {
        return lit;
    }

    void print(int lvl) {
        cout << string(lvl, '\t') << "int_lit: " << lit << '\n';
    }
};

class t_str_lit : public t_print_item {
  private:
    std::string* lit;
    
  public:
    t_str_lit(std::string* lit) : lit(lit) {  }

    std::string str_val() {
        return *lit;
    }

    void print(int lvl) {
        cout << string(lvl, '\t') << "str_lit: " << lit << '\n';
    }
};

class t_id : public t_expression {
  private:
    int id;
	//TODO Tipo?

  public:
    t_id(int id) : id(id) {  }

    int val() {
        int tmp = rand()%10;
        std::cerr << "assigned id " << id << " random value " << tmp << std::endl;
        return tmp;
    }

    void print(int lvl) {
        cout << string(lvl, '\t') << "id: " << id << ": " /*<< id_data[id]*/ << '\n'; //TODO
    }
};

namespace t_builtin_types {
     static t_id integer(-1); //TODO see correct way of initialization
};

class t_function {
  private:
    t_id id;
    
};

using t_functions = std::vector<t_function*>;
// class t_functions {
    
// };

class t_compound_statement {
   //TODO Fusionar con statements? 
};

class t_optional_statements {
   //TODO Fusionar con statments? 
};

class t_statement {
  public:
    virtual void print(int lvl) = 0;
};

using t_statements = std::vector<t_statement*>;
// class t_statements {
    
// };


using t_arguments = t_expressions;
// class t_arguments {
    
// };

using t_identifiers = std::vector<t_id*>;
// class t_identifiers {
    
// };

class t_type {
  //TODO Emilio hippie  
};

using t_read_item = std::string;
// class t_read_item {
    
// };

using t_read_list = std::vector<t_read_item*>;
// class t_read_list {

// };

class t_assignment : public t_statement {
  private:
    t_id* id;
    t_expression* exp;

  public:
    t_assignment(t_id* id, t_expression* exp) : id(id), exp(exp) {  }

    void print(int lvl) {
        cout << string(lvl, '\t') << "assignment\n";
        id->print(lvl+1);
        exp->print(lvl+1);
    }
};

//TODO only for testing
static t_assignment auxiliar(new t_id(0), new t_id(0));

using t_constants = std::vector<t_assignment*>;
// class t_constants {
    
// };

class t_declarations {
  public:
    std::vector<t_assignment*> constants;
    std::vector<t_id*> variables;

    t_declarations() : constants(), variables() {  }

    void add_constants(t_constants* consts) {
        constants.insert(constants.end(), consts->begin(), consts->end());
    }

    void add_identifiers(t_identifiers* ids, t_id* type) {
        for (auto id : *ids) {
            // register type in table
        }
        variables.insert(variables.end(), ids->begin(), ids->end());
    }

    void print(int lvl) {
        string tabs = string(lvl, '\t');
        cout << tabs << "declarations:\n";
        cout << tabs << "\tconstants\n";
        for (auto c : constants) {
            c->print(lvl+1);
        }
        cout << tabs << "\tvariables\n";
        for (auto v : variables) {
            v->print(lvl+1);
        }
    }
};

class T_program {
  public:
    t_functions* functions;
    t_declarations* declarations;
    t_statements* statements;

    T_program() {  }

    T_program(t_functions* funcs, t_declarations* decls, t_statements* stmts)
         : functions(funcs), declarations(decls), statements(stmts) {

    }

    void print(int lvl = 0) {
        cout << "Program\n";
        // functions->print(lvl+1);
        declarations->print(lvl+1);
        cout << "\n\tstatements:\n";
        for (auto st : *statements) {
            st->print(lvl+2);
        }
    }
};


#endif // AST_HPP

