%{
%}

%option noyywrap

DIGIT    [0-9]
ID       [a-zA-Z]{1,250}
STRING   \"[^\"]*\"

%%

{DIGIT}+    {
            printf("An integer: %s (%d)\n", yytext,
                    atoi(yytext));
            }

{DIGIT}+"."{DIGIT}*        {
            printf("A float: %s (%g)\n", yytext,
                    atof(yytext));
            }


include|int|float|if|else|while|return 	{
            printf("A keyword: %s\n", yytext);
            }

{ID}        printf("An identifier: %s\n", yytext);

{STRING} 	printf("A string: %s\n", yytext);


"!"|"&"|"*"|"+"|"-"|"/"|"<"|"="|">"|"?"|"^"|"|"|"~"|"=="|"!="|"<="|">="|"<<"|">>"|"&="|"|="|"^="|"+="|"-="|"++"|"--"|"&&"|"||" 	{
			printf("An operator: %s\n", yytext);
			}


"#"|"("|")"|","|";"|"["|"]"|"{"|"}" 	{
			printf("A separator: %s\n", yytext);
			}

[ \t\n]+          /* eat up whitespace */

.           printf("Unrecognized character: %s\n", yytext);

%%

int main(int /* argc */, char** /* argv */)
    {
    FlexLexer* lexer = new yyFlexLexer;
    while(lexer->yylex() != 0)
        ;
    return 0;
    }
