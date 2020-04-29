#include "types.hpp"

#include "namespace.hpp"
#include "function.hpp"

namespace AST {

namespace builtin {

const std::string t_type_int::llvm_name_ = "i32";

class t_int_plus_operator : public t_function {
  public:
    t_int_plus_operator() : t_function(t_int(),
                                       t_namespace::get_id("operator+"),
                                       {{t_int(), t_namespace::get_id(".lhs")},
                                        {t_int(), t_namespace::get_id(".rhs")}},
                                       nullptr,
                                       nullptr) {  }
    
    virtual std::string llvm_put_call(std::ostream& os,
                                      int& local_var_count,
                                      std::vector<std::string*> params) {
        assert(params.size() == 2);
        std::string ref = "%" + std::to_string(local_var_count++);
        os << "\t" << ref << " = add nsw i32 " << *params[0] << ", " 
           << *params[1] << "\n";
        return ref;
    }
};

const std::string t_type_str::llvm_name_ = "i8*";

void llvm_register_types() {
    t_namespace::get_id("integer")->register_as_type(new t_type_int()); //TODO por dios que alguien llame int a integer!!!!
    new t_int_plus_operator(); //TODO las funciones se registran solas lo cual es raro
    
    t_namespace::get_id("str")->register_as_type(new t_type_str());
}

} // namespace builtin

} // namespace AST
