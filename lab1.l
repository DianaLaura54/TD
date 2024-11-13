%{
    extern int yylval;
    #define NUMBER 257
%}

%%
[0-9]+	{
		yylval = atoi(yytext);
		return NUMBER;
	}

[+()\n] {
    	return *yytext;
}

[ \t] ;

.		printf("Unknown character")