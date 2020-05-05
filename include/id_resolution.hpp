#ifndef ID_RESOLUTION_HPP
#define ID_RESOLUTION_HPP

#include <vector>
#include "ast_defs.hpp"

namespace compiler {

namespace identifiers {

class NameResolution {
  public:
    static void Do();
    static void FirstPass();
    static void SecondPass();

  private:
    static ast::Fun* CallToFun(Id*, const std::vector<ast::Type*>& signature);
};

} // namespace identifiers

} // namespace compiler

#endif // ID_RESOLUTION_HPP

