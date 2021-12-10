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
%token INT
%token FLOAT
%token IF
%token ELSE
%token WHILE
%token RETURN
%token PREPROCESSOR_LINE
%token DACA
%token ATUNCI
%token SFDACA
%left '='
%left EQ
%left N_EQ
%left G_EQ
%left L_EQ
%left A_ASSIGN
%left S_ASSIGN
%left M_ASSIGN
%left D_ASSIGN
%left '>'
%left '<'
%left '+'
%left '-'
%left '*'
%left '/'

%%

program:
	%empty
	| function program
	| PREPROCESSOR_LINE program

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
	| instr_daca

instr_daca:
	DACA '(' ID ')' ATUNCI instruction_list SFDACA

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
	| '&' term

exp:
   term
   | CONST
   | term '=' exp
   | term A_ASSIGN exp
   | term S_ASSIGN exp
   | term M_ASSIGN exp
   | term D_ASSIGN exp
   | exp '>' exp
   | exp '<' exp
   | exp EQ exp
   | exp N_EQ exp
   | exp G_EQ exp
   | exp L_EQ exp
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
	IF '(' exp ')' '{' instruction_list '}'
	| IF '(' exp ')' '{' instruction_list '}' ELSE '{' instruction_list '}'

while_statement:
	WHILE '(' exp ')' '{' instruction_list '}'

return_statement:
	RETURN exp

%%

void yyerror(const char *s) {
	printf("Error: %s\n", s);
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
