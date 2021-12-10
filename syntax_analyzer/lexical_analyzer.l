%option noyywrap

NUMBER ([1-9][0-9]*)|0
FLOAT [0-9]+\.[0-9]+
ID       [a-zA-Z\.]+
STRING   \"[^\"]*\"

%%

{NUMBER}    return CONST;

{FLOAT} 	return CONST;

include|int|float|if|else|while|return|repeta|panacand|sfrepeta {
			registerKeyword(yytext);
			}

{ID}        registerIdentifier(yytext);

{STRING} 	registerString(yytext);


"!"|"&"|"*"|"+"|"-"|"/"|"<"|"="|">"|"?"|"^"|"|"|"~"|"=="|"!="|"<="|">="|"<<"|">>"|"&="|"|="|"^="|"+="|"-="|"++"|"--"|"&&"|"||" 	{
			registerOperator(yytext);
			}


"#"|"("|")"|","|";"|"["|"]"|"{"|"}" 	{
			registerSeparator(yytext);
			}

[ \t\n]+          /* eat up whitespace */

.           printf("Unrecognized character: %s\n", yytext);

%%
