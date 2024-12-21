%{

extern int yylval;
#define NUMBER 256
#define LETTER 257
#define WHILE 260
#define IF 261
#define ELSE 262
%}

%%

[0-9]+ { yylval = atoi(yytext); return NUMBER; }
[+()=\n] return *yytext;
while { return WHILE;}
if {return IF;}
else {return ELSE;}



