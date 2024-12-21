%{
#define LETTER 256
#define NUMBER 257
#define ELSE 260
#define IF 261
int yylval;
%}

%%

[0-9]+ { yylval = atoi(yytext); return NUMBER; }
if {return IF;}
else { return ELSE;}
[+*(){}=\n] { return *yytext;}

