#include <cstdio>
#include <cstdlib>
#include <memory>
#include <map>

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
class GenericASTNode
{
public:
    virtual ~GenericASTNode() = default;
    virtual void toString(){};
    virtual Value *codegen() = 0;
};

class StatementASTNode : public GenericASTNode
{
    unique_ptr<GenericASTNode> node;
    unique_ptr<GenericASTNode> nextNode;

public:
    StatementASTNode(unique_ptr<GenericASTNode> node, unique_ptr<GenericASTNode> nextNode)
    {
        this->node = std::move(node);
        this->nextNode = std::move(nextNode);
    }

    void toString() { return; }
    Value *codegen() { return nullptr; }
};

class NumberASTNode : public GenericASTNode
{
    int Val;

public:
    NumberASTNode(int Val)
    {
        this->Val = Val;
    }

    void toString() { return; }
    Value *codegen() { return nullptr; }
};

class VariableReadASTNode : public GenericASTNode
{
    string name;

public:
    VariableReadASTNode(char name[])
    {
        this->name = name;
    }

    void toString() { return; }
    Value *codegen() { return nullptr; }
};

class VariableDeclarationASTNode : public GenericASTNode
{
    string name;

public:
    VariableDeclarationASTNode(char name[])
    {
        this->name = name;
    }

    void toString() { return; }
    Value *codegen() { return nullptr; }
};

class VariableAssignASTNode : public GenericASTNode
{
    string varName;
    unique_ptr<GenericASTNode> value;

public:
    VariableAssignASTNode(char varName[], unique_ptr<GenericASTNode> value)
    {
        this->varName = varName;
        this->value = std::move(value);
    }

    void toString() { return; }
    Value *codegen() { return nullptr; }
};

class BinaryExprAST : public GenericASTNode
{
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
    Value *codegen() { return nullptr; }
};

class IfStatementAST : public GenericASTNode
{
    unique_ptr<GenericASTNode> Cond, TrueExpr, FalseExpr;

public:
    IfStatementAST(unique_ptr<GenericASTNode> Cond, unique_ptr<GenericASTNode> TrueExpr, unique_ptr<GenericASTNode> FalseExpr)
    {
        this->Cond = std::move(Cond);
        this->TrueExpr = std::move(TrueExpr);
        this->FalseExpr = std::move(FalseExpr);

        // only if statement
        if (this->FalseExpr == nullptr)
        {
            this->FalseExpr = make_unique<NumberASTNode>(0);
        }
    }

    void toString() { return; }
    Value *codegen() { return nullptr; }
};

class WhileStatementAST : public GenericASTNode
{
    unique_ptr<GenericASTNode> Cond, Body;

public:
    WhileStatementAST(
        unique_ptr<GenericASTNode> Cond,
        unique_ptr<GenericASTNode> Body)
    {
        this->Cond = std::move(Cond);
        this->Body = std::move(Body);
    }

    void toString() { return; }
    Value *codegen() { return nullptr; }
};

void CodeGenTopLevel(shared_ptr<GenericASTNode> AST_Root)
{
    // Create an anonymous function with no parameters
    vector<Type *> ArgumentsTypes(0);
    FunctionType *FT = FunctionType::get(Type::getInt32Ty(*TheContext), ArgumentsTypes, false);
    Function *F = Function::Create(FT, Function::ExternalLinkage, "main", TheModule.get());

    // Create a label 'entry' and set it to the current position in the builder
    BasicBlock *BB = BasicBlock::Create(*TheContext, "entry", F);
    Builder->SetInsertPoint(BB);

    // Generate the code for the body of the function and return the result
    if (Value *RetVal = AST_Root->codegen())
    {
        Builder->CreateRet(RetVal);
    }

    auto Filename = "output.ll";
    std::error_code EC;
    raw_fd_ostream dest(Filename, EC);

    if (EC)
    {
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
extern char identifier[255];

unique_ptr<GenericASTNode> Z();
unique_ptr<GenericASTNode> E_AS();  // Addition and Subtraction
unique_ptr<GenericASTNode> E_MDR(); // Multiplication, Division and Remainders
unique_ptr<GenericASTNode> E_IF();
unique_ptr<GenericASTNode> E_WHILE();
unique_ptr<GenericASTNode> T();
unique_ptr<GenericASTNode> VAR_DECL();
unique_ptr<GenericASTNode> VAR_ASSIGN();

unique_ptr<GenericASTNode> Statements();
unique_ptr<GenericASTNode> Statement();

unique_ptr<GenericASTNode> Z(){

 if(symbol==IF)
    {
        return E_IF();
    }
    else if(symbol==WHILE)
    {
        return E_WHILE();
    }
    else return E_AS();

}

unique_ptr<GenericASTNode> E_AS(){



};

unique_ptr<GenericASTNode> E_MDR(){
};

unique_ptr<GenericASTNode> E_IF()
{
 unique_ptr<GenericASTNode>p=E_AS();
 if (symbol == IF)
    {
        next_symbol();
        nodeType *rez1,*rez2;
        if (symbol == '(')
        {
            next_symbol();
            nodeType *cond = E_AS();
            if (symbol == ')')
            {
                next_symbol();
                if(symbol=='{')
                {
                 next_symbol();
                nodeType *body = E_AS();
                if(symbol=='}')
                {
                  next_symbol();
                if(symbol==ELSE)
                {
                  if(symbol=='{')
                  {
                    next_symbol();
                    nodeType *body2=E_AS();
                    if(symbol=='}')
                    {
             next_symbol();
            return make_unique<IfStatementAST>(cond,body,body2);
                    }
                  }
                }
                else
                {
                    next_symbol();
                    return make_unique<IfStatementAST>(cond,body);

                }
              }
            }
        }
    }
 }
};





unique_ptr<GenericASTNode> E_IF(){




}

unique_ptr<GenericASTNode> E_WHILE(){


}

unique_ptr<GenericASTNode> T(){





}
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

    next_symbol();
    CodeGenTopLevel(Z());

    return 0;
}