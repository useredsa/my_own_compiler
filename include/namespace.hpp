#ifndef NAMESPACE_HPP
#define NAMESPACE_HPP

#include <string>
#include <unordered_map>
#include "identifiers.hpp"

namespace AST {

using string = std::string;

class t_namespace;
struct t_unresolved_name;

class t_namespace {
  public:
    /**
     * @brief Registers a new identifier in the current namespace.
     */
    static t_id* new_id(const string& name);

    /**
     * @brief Gets the t_id associated with a name in the CURRENT namespace.
     * 
     * @returns a t_id constructed via t_id::undeclared() if the name wasn't declared yet.
     */
    static t_id* get_id(const string& name);

    /**
     * @brief Gets a t_unresolved_name which will be resolved at the end of the parsing.
     */
    static t_unresolved_name* pending_id(const string& name);

    /**
     * @brief Creates a new namespace on top of the current one.
     */
    static t_namespace* create();

    static inline t_namespace* current_namesp() {
        return current_namesp_;
    }

    static void switch_namespace(t_namespace* namesp);

    static void switch_to_parent_namespace();

  private:
    static t_namespace* current_namesp_;
    friend void resolve_names();

    t_namespace* parent;
    std::unordered_map<std::string, t_id*> identifiers;
    std::unordered_map<std::string, t_unresolved_name*> unresolved_names;
    std::unordered_map<std::string, t_unresolved_name*> resolved_names;
    std::unordered_map<std::string, t_id*> error_ids;

    t_namespace(t_namespace* parent = nullptr);
};

void resolve_names();

struct t_unresolved_name {
    union {
        string* unresolved_name;
        t_id* resolved_id;
    };

    t_unresolved_name(const string& name) : unresolved_name(new string(name)) {  }

    t_unresolved_name(t_id* id) : resolved_id(id) {  }

    inline t_id* id() {
        return resolved_id;
    }
};

} // namespace AST

#endif // NAMESPACE_HPP

