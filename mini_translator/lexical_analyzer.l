%{
	#include <string.h>
	#include "syntax_analyzer.tab.h"
%}

%option noyywrap

NUMBER -?([1-9][0-9]*)|0
FLOAT [0-9]+\.[0-9]+
ID       [a-zA-Z\.]+
STRING   \"[^\"]*\"
PREPROCESSOR_LINE ^#.*$

%%

{NUMBER}    {
				strcpy(yylval.varname, yytext);
				return CONST_NUMBER;
			}

{FLOAT} 	return CONST;

int 		return INT;

float 		return FLOAT;

if 			return IF;

else 		return ELSE;

while 		return WHILE;

return		return RETURN;

{ID}        {
				strcpy(yylval.varname, yytext);
				return ID;
			}

{STRING} 	return CONST;

{PREPROCESSOR_LINE} return PREPROCESSOR_LINE;

"==" 		return EQ; 

"!=" 		return N_EQ;

">=" 		return G_EQ;

"<=" 		return L_EQ;

"+=" 		return A_ASSIGN;

"-=" 		return S_ASSIGN;

"*=" 		return M_ASSIGN;

"/=" 		return D_ASSIGN;

"="|">"|"<"|"+"|"-"|"*"|"/"|";"|"("|")"|"{"|"}"|","|"&" {
			return yytext[0];
			}


[ \t\n]+          /* eat up whitespace */

.           printf("Unrecognized character: %s\n", yytext);

%%
