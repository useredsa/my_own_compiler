#include "builtin.hpp"

#include "expressions.hpp"
#include "llvm.hpp"

namespace compiler {

namespace builtin {

class IntType : public ast::Type {
  public:
    IntType(identifiers::Id* id) : Type(id) {}

    inline std::string llvm_name() override {
        return "i32";
    }

    inline int def_alignment() override {
        return 4;
    }
};

class IntTypeUnaMinus : public ast::Fun {
  public:
    static inline IntTypeUnaMinus* CreateAndRegister() {
        identifiers::Id* id = identifiers::GetId(".operator-");
        identifiers::AddNameScope();
        auto* ptr = new IntTypeUnaMinus(id);
        identifiers::AbandonCurrentNameScope();
        return ptr;
    }
    
    virtual std::string llvm_put_call(std::ostream& os,
                                      int& local_var_count,
                                      const std::vector<llvm::ComputedExp*>& params) override {
        assert(params.size() == 1);
        std::string ref = "%" + std::to_string(local_var_count++);
        os << "\t" << ref << " = sub nsw i32 0, " << params[0]->val << "\n";
        return ref;
    }

  private:
    IntTypeUnaMinus(identifiers::Id* id)
        : Fun(id,
              ast::RType(IntTypeId()),
              {
                  new ast::Var(identifiers::NewId(".op"), ast::RType(IntTypeId())),
              },
              {},
              {}) {  }
};

class IntTypeBinPlus : public ast::Fun {
  public:
    static inline IntTypeBinPlus* CreateAndRegister() {
        identifiers::Id* id = identifiers::GetId(".operator+");
        identifiers::AddNameScope();
        auto* ptr = new IntTypeBinPlus(id);
        identifiers::AbandonCurrentNameScope();
        return ptr;
    }
    
    virtual std::string llvm_put_call(std::ostream& os,
                                      int& local_var_count,
                                      const std::vector<llvm::ComputedExp*>& params) override {
        assert(params.size() == 2);
        std::string ref = "%" + std::to_string(local_var_count++);
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
    
    virtual std::string llvm_put_call(std::ostream& os,
                                      int& local_var_count,
                                      const std::vector<llvm::ComputedExp*>& params) override {
        assert(params.size() == 2);
        std::string ref = "%" + std::to_string(local_var_count++);
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
    
    virtual std::string llvm_put_call(std::ostream& os,
                                      int& local_var_count,
                                      const std::vector<llvm::ComputedExp*>& params) override {
        assert(params.size() == 2);
        std::string ref = "%" + std::to_string(local_var_count++);
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
    
    virtual std::string llvm_put_call(std::ostream& os,
                                      int& local_var_count,
                                      const std::vector<llvm::ComputedExp*>& params) override {
        assert(params.size() == 2);
        std::string ref = "%" + std::to_string(local_var_count++);
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

class StrType : public ast::Type {
  public:
    StrType(identifiers::Id* id) : Type(id) {}

    inline std::string llvm_name() override {
        return "i8*";
    }

    inline int def_alignment() override {
        return 8;
    }
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
    
    virtual std::string llvm_put_call(std::ostream& os,
                                      int& local_var_count,
                                      const std::vector<llvm::ComputedExp*>& params) override {
        assert(params.size() == 2);
        std::string len1 = "%" + std::to_string(local_var_count++);
        std::string len2 = "%" + std::to_string(local_var_count++);
        os << "\t" << len1 << " = call i64 @strlen(i8* " << params[0]->val << ")\n";
        os << "\t" << len2 << " = call i64 @strlen(i8* " << params[1]->val << ")\n";
        std::string lenNoNull = "%" + std::to_string(local_var_count++);
        os << "\t" << lenNoNull << " = add nsw i64 " << len1 << ", " << len2 << "\n";
        std::string len = "%" + std::to_string(local_var_count++);
        os << "\t" << len << " = add nsw i64 " << lenNoNull << ", 1" << "\n";
        std::string ref = "%" + std::to_string(local_var_count++);
        
        os << "\t" << ref << " = alloca i8, i64 " << len << "\n";
        
        os << "\t%" << local_var_count++ << " = call i8* @strcpy(i8* " << ref << ", "
           << "i8*" << params[0]->val << ")\n";
        std::string end = "%" + std::to_string(local_var_count++);
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

void RegisterBuiltins() {
    identifiers::AddNameScope(identifiers::kAcronological);
    new IntType(identifiers::NewId("int"));
    new StrType(identifiers::NewId("str"));
    IntTypeBinPlus::CreateAndRegister();
    IntTypeBinMinus::CreateAndRegister();
    IntTypeBinSlash::CreateAndRegister();
    IntTypeBinAsterisk::CreateAndRegister();
    IntTypeUnaMinus::CreateAndRegister();
    StrTypeBinPlus::CreateAndRegister();
    identifiers::NewId("main");
    new ast::StrLit(new std::string("%d"), "@.io.int");
    new ast::StrLit(new std::string("%s"), "@.io.str");
}

} // namespace builtin

} // namespace compiler

