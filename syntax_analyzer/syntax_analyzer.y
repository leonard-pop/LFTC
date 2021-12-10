%{
	#include <string.h>
	#include <stdio.h>
	#include <stdlib.h>

	extern int yylex();
	extern int yyparse();
	extern FILE *yyin;
	extern int nrLine;
	void yyerror(const char *s);
%}

%token ID
%token CONST
%left exp '=' exp
%left exp '==' exp
%left exp '!=' exp
%left exp '>=' exp
%left exp '<=' exp
%left exp '+=' exp
%left exp '-=' exp
%left exp '*=' exp
%left exp '/=' exp
%left exp '>' exp
%left exp '<' exp
%left exp '+' exp
%left exp '-' exp
%left exp '*' exp
%left exp '/' exp

%%

program:
   function_list

function_list:
	%empty
	| function function_list

function:
	type ID '(' parameter_list ')' '{' instruction_list '}'

type:
	INT
	| FLOAT

parameter_list:
	%empty
	| parameter_declaration
	| parameter_declaration ',' parameter_list

parameter_declaration:
	type ID

instruction_list:
	%empty
	| instruction instruction_list

instruction:
	variable_declaration ';'
	| exp ';'
	| function_call ';'
	| if_statement
	| while_statement
	| return_statement ';'

variable_declaration:
	type declaration_id_list

declaration_id_list:
	term
	| term '=' exp
	| term ',' declaration_id_list
	| term '=' exp ',' declaration_id_list

term:
	ID
	| '*' term

exp:
   term
   | '&' term
   | '*' term
   | CONST
   | term '=' exp
   | term '+=' exp
   | term '-=' exp
   | term '*=' exp
   | exp '/=' exp
   | exp '>' exp
   | exp '<' exp
   | exp '==' exp
   | exp '!=' exp
   | exp '>=' exp
   | exp '<=' exp
   | exp '+' exp
   | exp '-' exp
   | exp '*' exp
   | exp '/' exp
   | '(' exp ')'

function_call:
	ID '(' argument_list ')'

argument_list:
	%empty
	| exp
	| exp ',' argument_list

if_statement:
	'if' '(' exp ')' '{' instruction_list '}'
	| 'if' '(' exp ')' '{' instruction_list '}' 'else' '{' instruction_list '}'

while_statement:
	'while' '(' exp ')' '{' instruction_list '}'

return_statement:
	'return' exp

%%

void yyerror(const char *s) {
	printf("Syntax error: %s\n", s);
	exit(1);
}

int main(int argc, char *argv[]) {
	++argv, --argc;

	if(argc > 0){
		yyin = fopen(argv[0], "r");
	} else{
		yyin = stdin;
	}

	while (!feof(yyin)) {
		yyparse();
	}

	printf("No errors found\n");

	return 0;
}
