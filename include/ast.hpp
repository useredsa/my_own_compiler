#ifndef AST_HPP
#define AST_HPP

#include <string>
#include <vector>
#include <iostream> //TODO remove

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
};

class t_int_lit : public t_expression {
  public:
    const int lit;

    t_int_lit(int lit) : lit(lit) {  }

    int val() {
        return lit;
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
};

//TODO Diría que hereda de expression pero necesitaría un valor val()
//el valor val es solo para probar, evidentemente no se puede saber en tiempo
// de compilación
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
};

class t_program {
    
};

class t_function {
  private:
    t_id id;
    
};

using t_functions = std::vector<t_function*>;
// class t_functions {
    
// };

class t_compound_statement {
    
};

class t_optional_statements {
    
};

class t_statement {
    
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
    t_id *id;
    t_expression *exp;

  public:
    t_assignment(t_id *id, t_expression *exp) : id(id), exp(exp) {  }
};

using t_constants = std::vector<t_assignment*>;
// class t_constants {
    
// };

class t_declarations {
  public:
    t_constants* consts;
    //TODO
};


#endif // AST_HPP

