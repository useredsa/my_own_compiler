#include "types.hpp"
#include "llvm.hpp"

using std::string;

namespace compiler {

namespace builtin {

class IntTypeBinPlus : public ast::Fun {
  public:
    static inline IntTypeBinPlus* CreateAndRegister() {
        identifiers::Id* id = identifiers::GetId(".operator+");
        identifiers::AddNameScope();
        auto* ptr = new IntTypeBinPlus(id);
        identifiers::AbandonCurrentNameScope();
        return ptr;
    }
    
    virtual string llvm_put_call(std::ostream& os,
                                 int& local_var_count,
                                 const std::vector<llvm::ComputedExp*>& params) override {
        assert(params.size() == 2);
        string ref = "%" + std::to_string(local_var_count++);
        os << "\t" << ref << " = add nsw i32 " << params[0]->val << ", " 
           << params[1]->val << "\n";
        return ref;
    }

  private:
    IntTypeBinPlus(identifiers::Id* id)
        : Fun(id,
              ast::RType(IntTypeId()),
              {
                  new ast::Var(identifiers::NewId(".lhs"), ast::RType(IntTypeId())),
                  new ast::Var(identifiers::NewId(".rhs"), ast::RType(IntTypeId()))
              },
              {},
              {}) {  }
};

class IntTypeBinMinus : public ast::Fun {
  public:
    static inline IntTypeBinMinus* CreateAndRegister() {
        identifiers::Id* id = identifiers::GetId(".operator-");
        identifiers::AddNameScope();
        auto* ptr = new IntTypeBinMinus(id);
        identifiers::AbandonCurrentNameScope();
        return ptr;
    }
    
    virtual string llvm_put_call(std::ostream& os,
                                 int& local_var_count,
                                 const std::vector<llvm::ComputedExp*>& params) override {
        assert(params.size() == 2);
        string ref = "%" + std::to_string(local_var_count++);
        os << "\t" << ref << " = sub nsw i32 " << params[0]->val << ", " 
           << params[1]->val << "\n";
        return ref;
    }

  private:
    IntTypeBinMinus(identifiers::Id* id)
        : Fun(id,
              ast::RType(IntTypeId()),
              {
                  new ast::Var(identifiers::NewId(".lhs"), ast::RType(IntTypeId())),
                  new ast::Var(identifiers::NewId(".rhs"), ast::RType(IntTypeId()))
              },
              {},
              {}) {  }
};

class IntTypeBinAsterisk : public ast::Fun {
  public:
    static inline IntTypeBinAsterisk* CreateAndRegister() {
        identifiers::Id* id = identifiers::GetId(".operator*");
        identifiers::AddNameScope();
        auto* ptr = new IntTypeBinAsterisk(id);
        identifiers::AbandonCurrentNameScope();
        return ptr;
    }
    
    virtual string llvm_put_call(std::ostream& os,
                                 int& local_var_count,
                                 const std::vector<llvm::ComputedExp*>& params) override {
        assert(params.size() == 2);
        string ref = "%" + std::to_string(local_var_count++);
        os << "\t" << ref << " = mul nsw i32 " << params[0]->val << ", " 
           << params[1]->val << "\n";
        return ref;
    }

  private:
    IntTypeBinAsterisk(identifiers::Id* id)
        : Fun(id,
              ast::RType(IntTypeId()),
              {
                  new ast::Var(identifiers::NewId(".lhs"), ast::RType(IntTypeId())),
                  new ast::Var(identifiers::NewId(".rhs"), ast::RType(IntTypeId()))
              },
              {},
              {}) {  }
};

class IntTypeBinSlash : public ast::Fun {
  public:
    static inline IntTypeBinSlash* CreateAndRegister() {
        identifiers::Id* id = identifiers::GetId(".operator/");
        identifiers::AddNameScope();
        auto* ptr = new IntTypeBinSlash(id);
        identifiers::AbandonCurrentNameScope();
        return ptr;
    }
    
    virtual string llvm_put_call(std::ostream& os,
                                 int& local_var_count,
                                 const std::vector<llvm::ComputedExp*>& params) override {
        assert(params.size() == 2);
        string ref = "%" + std::to_string(local_var_count++);
        os << "\t" << ref << " = sdiv i32 " << params[0]->val << ", " 
           << params[1]->val << "\n";
        return ref;
    }

  private:
    IntTypeBinSlash(identifiers::Id* id)
        : Fun(id,
              ast::RType(IntTypeId()),
              {
                  new ast::Var(identifiers::NewId(".lhs"), ast::RType(IntTypeId())),
                  new ast::Var(identifiers::NewId(".rhs"), ast::RType(IntTypeId()))
              },
              {},
              {}) {  }
};

class StrTypeBinPlus : public ast::Fun {
  public:
    static inline StrTypeBinPlus* CreateAndRegister() {
        identifiers::Id* id = identifiers::GetId(".operator+");
        identifiers::AddNameScope();
        auto* ptr = new StrTypeBinPlus(id);
        identifiers::AbandonCurrentNameScope();
        return ptr;
    }
    
    virtual string llvm_put_call(std::ostream& os,
                                 int& local_var_count,
                                 const std::vector<llvm::ComputedExp*>& params) override {
        assert(params.size() == 2);
        string len1 = "%" + std::to_string(local_var_count++);
        string len2 = "%" + std::to_string(local_var_count++);
        os << "\t" << len1 << " = call i64 @strlen(i8* " << params[0]->val << ")\n";
        os << "\t" << len2 << " = call i64 @strlen(i8* " << params[1]->val << ")\n";
        string lenNoNull = "%" + std::to_string(local_var_count++);
        os << "\t" << lenNoNull << " = add nsw i64 " << len1 << ", " << len2 << "\n";
        string len = "%" + std::to_string(local_var_count++);
        os << "\t" << len << " = add nsw i64 " << lenNoNull << ", 1" << "\n";
        string ref = "%" + std::to_string(local_var_count++);
        
        os << "\t" << ref << " = alloca i8, i64 " << len << "\n";
        
        os << "\t%" << local_var_count++ << " = call i8* @strcpy(i8* " << ref << ", "
           << "i8*" << params[0]->val << ")\n";
        string end = "%" + std::to_string(local_var_count++);
        os << "\t" << end << " = getelementptr i8, i8* " << ref << ", i64 " << len1 << "\n";
        os << "\t%" << local_var_count++ << " = call i8* @strcpy(i8* " << end << ", "
           << "i8*" << params[1]->val << ")\n";
        return ref;
    }
  private:
    StrTypeBinPlus(identifiers::Id* id)
        : Fun(id,
          ast::RType(StrTypeId()),
          { 
              new ast::Var(identifiers::NewId(".lhs"), ast::RType(StrTypeId())),
              new ast::Var(identifiers::NewId(".rhs"), ast::RType(StrTypeId()))
          },
          {},
          {}) {  }
};

void RegisterTypes() {
    new IntType(identifiers::NewId("int"));
    new StrType(identifiers::NewId("str"));
    IntTypeBinPlus::CreateAndRegister();
    IntTypeBinMinus::CreateAndRegister();
    IntTypeBinSlash::CreateAndRegister();
    IntTypeBinAsterisk::CreateAndRegister();
    StrTypeBinPlus::CreateAndRegister();
}

} // namespace builtin

} // namespace compiler

