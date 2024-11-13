%{
#define NUMBER 256
int yylval;
%}

%%

[0-9]+ { yylval = atoi(yytext); return NUMBER; }
[+()=\n] return *yytext;

clang++-17 -g -O3 main.cpp lexer.cpp \
`llvm-config-17 --cxxflags --ldflags --system-libs --libs core` \
-o main -ll