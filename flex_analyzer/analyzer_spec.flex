%{
%}

%option noyywrap

DIGIT    [0-9]
ID       [a-zA-Z\.]+
STRING   \"[^\"]*\"

%%

{DIGIT}+    registerNumber(yytext);

{DIGIT}+"."{DIGIT}*        {
			registerFloat(yytext);
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
