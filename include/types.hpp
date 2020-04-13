#ifndef TYPES_HPP
#define TYPES_HPP

#include <iostream>
#include <string>
#include "type.hpp"
#include "identifiers.hpp"

namespace AST {

namespace builtin {

inline t_id* t_int() {
    static t_id* ptr = nullptr;
    if (ptr == nullptr)
        ptr = t_id::named("integer");
    return ptr;
}

class t_type_int : public t_type { //TODO transform to singleton?
  public:
    inline const std::string& llvm_name() {
        return llvm_name_;
    }

  private:
    static const std::string llvm_name_;
};

inline t_id* t_str() {
    static t_id* ptr = nullptr;
    if (ptr == nullptr)
        ptr = t_id::named("str");
    return ptr;
}

class t_type_str : public t_type {
  public:
    inline const std::string& llvm_name() {
        return llvm_name_;
    }

  private:
    static const std::string llvm_name_;
};

void llvm_register_types();

} // namespace builtin

} // namespace AST

#endif // TYPES_HPP

