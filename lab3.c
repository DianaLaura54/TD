#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include <stddef.h>
extern int yylval;
#define NUMBER 256
#define LETTER 257
#define WHILE 260
#define IF 261
#define ELSE 262
#include <stdarg.h>


/*
1+2+3

push 1
push 2
add
push 3
add
*/

/*
1+(2+3)

push 1
push 2
push 3
add
add
*/

/*
a=1+2
b=a+3

push 1
push 2
add
pop a
push a
push 3
add
pop b
*/



int symbol, yylval;
nodeType *T();
nodeType *E();
nodeType *id(int i);
nodeType *opr(int oper, int nops, ...);
nodeType *con(int value);

// Function to get the next symbol

void next_symbol()
{
    symbol = yylex();
}
// Implementation of yyerror
void yyerror(const char *s)
{
    fprintf(stderr, "Error: %s\n", s);
}

nodeType *id(int i)
{
    nodeType *p;
    /* allocate node */
    if ((p = malloc(sizeof(idNodeType))) == NULL)
        yyerror("out of memory");
    /* copy information */
    p->type = typeId;
    p->id.i = i;
    return p;
}

nodeType *opr(int oper, int nops, ...)
{
    va_list ap;
    nodeType *p;
    size_t size;
    int i;

    /* allocate node */
    size = sizeof(oprNodeType) + (nops - 1) * sizeof(nodeType *);
    if ((p = malloc(size)) == NULL)
        yyerror("out of memory");
    /* copy information */
    p->type = typeOpr;
    p->opr.oper = oper;
    p->opr.nops = nops;
    va_start(ap, nops);
    for (i = 0; i < nops; i++)
        p->opr.op[i] = va_arg(ap, nodeType *);
    va_end(ap);

    return p;
}
nodeType *con(int value)
{
    nodeType *p;

    /* allocate node */
    if ((p = malloc(sizeof(conNodeType))) == NULL)
        yyerror("out of memory");
    /* copy information */
    p->type = typeCon;
    p->con.value = value;
    return p;
}

// while (1+2+3) 11+(12+13)*/

/*
T :: = NUMBER
   | var
   | '(' E ')'.
*/
nodeType *T()
{
    nodeType *acc = E();
    if (symbol == NUMBER)
    {
        int acc = con(yylval);
        next_symbol();
        return acc;
    }

     if (symbol == '(')
    {
        next_symbol();
        nodeType *rez = E();
        if(symbol==')')
        {
            next_symbol();
            return rez;
        }
    }
     if(symbol==yylval)
    {
        nodeType *rez=yylval;
      next_symbol();
      return rez;
    }
    
   
}

/*
E :: = while '('E')' E
   | if '('E')' E else E
   | var '=' E
   | T('+'T)*.
*/
nodeType *E()
{

    if (symbol == WHILE)
    {
        next_symbol();
        nodeType *rez;
        if (symbol == '(')
        {
            next_symbol();
            rez = E();
            if (symbol == ')')
            {
                next_symbol();
                nodeType *rez2 = E();
                nodeType *rez3 = opr(WHILE, 2, rez, rez2);
                return rez3;
            }
        }
    }

    if (symbol == '+')
    {
        next_symbol();
        nodeType *acc = T();
        nodeType *rez = opr('+', 2, T(), acc);
        return rez;
    }

   if (symbol=='=')
    {

nodeType *acc=yylval;
next_symbol();
nodeType *rez=opr(symbol,2,acc,E);
return rez;
        
    }
 ///if '('E')' E else E
    if (symbol == IF)
    {
        next_symbol();
        nodeType *rez1,*rez2;
        if (symbol == '(')
        {
            next_symbol();
            nodeType *cond = E();
            if (symbol == ')')
            {

                next_symbol();
                nodeType *body = E();
                if(symbol == ELSE){
                    next_symbol();
                    nodeType *elsebody = E();
                  rez2=opr(IF,3,cond,body,elsebody);
                  return rez2;
                }
                else
                {
                    rez1=opr(IF,2,cond,body);
                    return rez1;
                }
            }
        }
    }
 
}

parser()
{
    ex(E());
    if (symbol != '\n')
        error();
        
}





int main()
{
next_symbol();
while(symbol!=0)
{
parser();
next_symbol();
}

}
