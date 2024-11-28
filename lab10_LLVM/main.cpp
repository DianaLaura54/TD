#include <cstdio>
#include <cstdlib>
#include <memory>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#define LETTER 256
#define NUMBER 256
#define WHILE 260
#define IF 261
#include <stdarg.h>
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
    virtual void toString() {};
    virtual Value *codegen() = 0;
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

class BinaryExprAST : public GenericASTNode
{
    char Op;
    unique_ptr<GenericASTNode> LHS, RHS;

public:
    BinaryExprAST(char Op, unique_ptr<GenericASTNode> LHS, unique_ptr<GenericASTNode> RHS)
    {
        this->Op = Op;
        this->LHS = move(LHS);
        this->RHS = move(RHS);
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
        this->Cond = move(Cond);
        this->TrueExpr = move(TrueExpr);
        this->FalseExpr = move(FalseExpr);

        // only if statement
        if (this->FalseExpr == nullptr)
        {
            this->FalseExpr = make_unique<NumberASTNode>(0);
        }
    }

    void toString() { return; }
    Value *codegen()
    {
        BasicBlock *BB1 = BasicBlock::Create(*TheContext, "then", F);
        BasicBlock *BB2 = BasicBlock::Create(*TheContext, "else", F);
        BasicBlock *BB3 = BasicBlock::Create(*TheContext, "merge", F);
        Builder->SetInsertPoint(BB1);
        Builder->SetInsertPoint(BB2);
        Builder->SetInsertPoint(BB3);
        Value *zeroValue = ConstantInt::get(*TheContext, APInt(32, 0, true));
        compareResult = Builder->CreateICmpNE(cond, zeroValue, "cond");
        Builder->CreateCondBr(compareResult, BB1, BB2);
    }
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
// Z ::= E_AS | E_IF.
// E_IF ::= if '('E_AS ')''{'E_AS '}'else '{'E_AS '}'
// E_AS ::= E_MDR ('+'| '-'E_MDR)*.
// E_MDR ::= T ('*'| '/'| '%'T)*.
// T ::= i | '('E_AS ')'.
unique_ptr<GenericASTNode> Z();
unique_ptr<GenericASTNode> E_AS();
unique_ptr<GenericASTNode> E_MDR();
unique_ptr<GenericASTNode> E_IF();
unique_ptr<GenericASTNode> T();

unique_ptr<GenericASTNode> Z()
{
    unique_ptr<GenericASTNode>p=E_AS();
    unique_ptr<GenericASTNode>p2=E_IF();





};

unique_ptr<GenericASTNode> E_AS()
{
    unique_ptr<GenericASTNode> p = E_MDR();
    while (symbol == '+' || symbol == '-')
    {
        char symb = symbol;
        next_symbol();
        unique_ptr<GenericASTNode> p2 = E_MDR();
        p = make_unique<BinaryExprAST>(symb, p, p2);
    }
    return p;
};

unique_ptr<GenericASTNode> E_IF()
{



};


unique_ptr<GenericASTNode> E_MDR()
{
    unique_ptr<GenericASTNode> p = T();
    while (symbol == '*' || symbol == '/' || symbol == '%')
    {
        char symb = symbol;
        next_symbol();
        unique_ptr<GenericASTNode> p2 = T();
        p = make_unique<BinaryExprAST>(symb, p, p2);
    }
    return p;
};



unique_ptr<GenericASTNode> T()
{
    if (symbol == NUMBER)
    {
        int number = yylval;
        next_symbol();
        return make_unique<NumberASTNode>(number);
    }
    else if (symbol == '(')
    {
        next_symbol();
        unique_ptr<GenericASTNode> p = E_AS();
        if (!symbol == ')')
        {
            return nullptr;
        }
        else
        {
            return p;
        }
    }
};

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