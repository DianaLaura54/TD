#include <cstdio>
#include <cstdlib>
#include <memory>

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/NoFolder.h"
#include "llvm/Support/raw_ostream.h"

using namespace std;
using namespace llvm;

#define NUMBER 256

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

    void toString() { return; }
    Value* codegen() { return nullptr; }
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

    void toString() { return; }
    Value* codegen() { return nullptr; }
};

void CodeGenTopLevel(unique_ptr<GenericASTNode> AST_Root)
{
    // Create an anonymous function with no parameters
    vector<Type *> ArgumentsTypes(0);
    FunctionType *FT = FunctionType::get(Type::getInt32Ty(*TheContext), ArgumentsTypes, false);
    Function *F = Function::Create(FT, Function::ExternalLinkage, "main", TheModule.get());

    // Create a label 'entry' and set it to the current position in the builder
    BasicBlock *BB = BasicBlock::Create(*TheContext, "entry", F);
    Builder->SetInsertPoint(BB);

    // Generate the code for the body of the function and return the result
    if (Value *RetVal = AST_Root->codegen()) {
        Builder->CreateRet(RetVal);
    }

    auto Filename = "output.ll";
    std::error_code EC;
    raw_fd_ostream dest(Filename, EC);

    if (EC) {
        errs() << "Could not open file: " << EC.message();
        return;
    }

    F->print(errs());
    F->print(dest);
    F->eraseFromParent();
}

//===----------------------------------------------------------------------===//
// Parser
//===----------------------------------------------------------------------===//
int yylex();
int symbol;
extern int yylval;

unique_ptr<GenericASTNode> Z();
unique_ptr<GenericASTNode> E_AS(); // Addition and subtraction
unique_ptr<GenericASTNode> E_MDR(); // Multiplication, Division and Remainder
unique_ptr<GenericASTNode> T();

void next_symbol()
{
	symbol = yylex();
}

//===----------------------------------------------------------------------===//
// main function
//===----------------------------------------------------------------------===//
int main()
{
    InitializeModule();

    return 0;
}