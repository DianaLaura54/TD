#include <cstdio>
#include <cstdlib>
#include <memory>

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/NoFolder.h"

using namespace std;
using namespace llvm;

static unique_ptr<LLVMContext> TheContext;
static unique_ptr<IRBuilder<NoFolder>> Builder;
static unique_ptr<Module> TheModule;

static void InitializeModule()
{
    TheContext = std::make_unique<LLVMContext>();
    TheModule = std::make_unique<Module>("MyModule", *TheContext);
    Builder = std::make_unique<IRBuilder<NoFolder>>(*TheContext);
}

//===----------------------------------------------------------------------===//
// AST nodes
//===----------------------------------------------------------------------===//
class GenericASTNode {
public:
    virtual ~GenericASTNode() = default;
    virtual void toString() {};
    virtual Value *codegen() = 0;
};

class NumberASTNode : public GenericASTNode {
  int Val;

public:
    NumberASTNode(int Val)
    {
        this->Val = Val;
    }
    void toString() { printf("%d",Val); }
    Value* codegen() { return ConstantInt::get(*TheContext, APInt(32, Val, true)); }
};

class BinaryExprAST : public GenericASTNode {
    char Op;
    unique_ptr<GenericASTNode> LHS, RHS;

public:
    BinaryExprAST(char Op, unique_ptr<GenericASTNode> LHS, unique_ptr<GenericASTNode> RHS)
    {
        this->Op = Op;
        this->LHS = std::move(LHS);
        this->RHS = std::move(RHS);
    }

   void toString() { printf("%d", Op); LHS->codegen(); RHS->codegen(); }

    Value* codegen() {
    Value *L = LHS->codegen();
    Value *R = RHS->codegen();

    if (!L || !R) return nullptr;

    switch (Op) {
    case '+':
        return Builder->CreateAdd(L, R, "addtmp");
    case '-':
        return Builder->CreateSub(L, R, "subtmp");
    case '*':
        return Builder->CreateMul(L, R, "multmp");
    case '/':
        return Builder->CreateSDiv(L, R, "divtmp");
    default:
        fprintf(stderr, "Unknown binary operator\n");
        return nullptr;
    }
}

};

void CodeGenTopLevel(unique_ptr<GenericASTNode> AST_Root)
{
    // Create an anonymous function with no parameters
    vector<Type *> ArgumentsTypes(0);
    FunctionType *FT = FunctionType::get(Type::getInt32Ty(*TheContext), ArgumentsTypes, false);
    Function *F = Function::Create(FT, Function::InternalLinkage, "__anon_expr", TheModule.get());

    // Create a label 'entry' and set it to the current position in the builder
    BasicBlock *BB = BasicBlock::Create(*TheContext, "entry", F);
    Builder->SetInsertPoint(BB);

    // Generate the code for the body of the function and return the result
    if (Value *RetVal = AST_Root->codegen()) {
        Builder->CreateRet(RetVal);
    }

    F->print(errs());
    F->eraseFromParent();
}

//===----------------------------------------------------------------------===//
// main function
//===----------------------------------------------------------------------===//
int main()
{
    InitializeModule();

    // Create an AST by hand and generate the code from it
    //unique_ptr<GenericASTNode> astRoot;
    //CodeGenTopLevel(move(astRoot));

    return 0;
}