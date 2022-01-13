%{
	#include <string.h>
	#include <stdio.h>
	#include <stdlib.h>

	extern int yylex();
	extern int yyparse();
	extern FILE *yyin;
	extern int nrLine;
	void yyerror(const char *s);

	char *variable_names[256];
	int variable_count = 0;
	int tempnr = 0;

	void registerVariable(char *variable_name) {
		int size = strlen(variable_name) + 1;

		variable_names[variable_count] = (char*)malloc(size *
			sizeof(char));
		strcpy(variable_names[variable_count], variable_name);

		variable_count++;
	}

	void newTempName(char* s) {
		sprintf(s, "temp%d", tempnr);
		registerVariable(s);
		tempnr++;
	}

	char tempbuffer[256];
	const char *ADD_ASM_FORMAT = "\tmov eax, %s\n\tadd eax, %s\n\tmov %s, eax\n",
		*SUB_ASM_FORMAT = "\tmov eax, %s\n\tsub eax, %s\n\tmov %s, eax\n";
%}

%code requires {
	typedef struct {
		char code[250];
		char varn[30];
	} attributes;
}

%union {
	char varname[30];
	attributes attrib;
	char strCode[250];
}

%token <varname> ID
%token <varname> CONST_NUMBER
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

%type <attrib> term
%type <attrib> exp
%type <attrib> argument_list

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
	| assignment ';'
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
	ID {
		registerVariable($1);
	}
	| term '=' exp
	| term ',' declaration_id_list
	| term '=' exp ',' declaration_id_list

term:
	ID {
		strcpy($$.code, "");
		sprintf($$.varn, "[%s]", $1);
	}
	| CONST_NUMBER {
		strcpy($$.code, "");
		strcpy($$.varn, $1);
	}
	| '*' term
	| '&' term

assignment:
	ID '=' exp {
		printf("%s\n", $3.code);
		printf("\tmov eax, %s\n", $3.varn);
		printf("\tmov [%s], eax\n", $1);
	}

exp:
   term {
		strcpy($$.code, $1.code);
		strcpy($$.varn, $1.varn);
	}
	| CONST
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
	| exp '+' exp {
		newTempName(tempbuffer);
		sprintf($$.varn, "[%s]", tempbuffer);

		sprintf($$.code, "%s\n%s\n", $1.code, $3.code);
		sprintf(tempbuffer, ADD_ASM_FORMAT, $1.varn, $3.varn,
			$$.varn);
		strcat($$.code, tempbuffer);
	}
	| exp '-' exp
	| exp '*' exp
	| exp '/' exp
	| '(' exp ')'

function_call:
	ID '(' argument_list ')' {
		if(!strcmp("printf", $1)) {
			printf("\tmov rdi, NUMBER_FORMAT\n\txor rsi, rsi\n\tmov esi, %s\n\txor rax, rax\n\tcall printf\n",
				$3.varn);
		}
	}

argument_list:
	%empty
	| CONST ',' term {
		strcpy($$.varn, $3.varn);
	}
//	| exp
//	| exp ',' argument_list

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

void cleanup() {
	for(int i = 0; i < variable_count; i++) {
		free(variable_names[i]);
	}
}

int main(int argc, char *argv[]) {
	++argv, --argc;

	if(argc > 0){
		yyin = fopen(argv[0], "r");
	} else{
		yyin = stdin;
	}

	printf("BITS 64\nextern printf\nglobal main\nmain:\n");

	while (!feof(yyin)) {
		yyparse();
	}

	//printf("No errors found\n");

	puts("\n\tmov rax, 60\n\tsyscall");
	puts("\nsection .data");

	for(int i = 0; i < variable_count; i++) {
		printf("\t%s dd 0\n", variable_names[i]);
	}

	puts("\tNUMBER_FORMAT db '%d',10,0");

	cleanup();

	return 0;
}
