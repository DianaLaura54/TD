#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#define LETTER 256
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

int symbol, yylval;

// Function to get the next symbol

void next_symbol() {
    symbol = yylex();
}

nodeType *opr(int oper, int nops, ...); 

/*E::=T('+'T)*   */
/* | while '(' E ')' E
/* | if '(' E ')' E else E

*/
nodeType *E(){
nodeType *acc=T();
while(symbol=='+')
{
    next_symbol();
    acc=opr('+',2,acc,T());
}
return acc;
}


parser(){
ex(E());
if(symbol!='\n')
error();
}


//while (1+2+3) 11+(12+13)*/

//T-> i | (E)*/
nodeType *T(){


return acc;
}


