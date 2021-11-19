#include <iostream>
#include <fstream>
#include "MDO.h"
#include "IteratorMDO.h"
#include <vector>
//#include <map>

void registerNumber(string buffer);
void registerIdentifier(string buffer);
void registerString(string buffer);
void registerSeparator(string buffer);
void registerOperator(string buffer);
void registerFloat(string buffer);

#include "lex.yy.cc"

#define MAX_ID_SIZE 250
#define ID_CODE 0
#define CONST_CODE 1

using namespace std;

bool compare(string first, string second) {
	if (first <= second) {
		return true;
	}
	else {
		return false;
	}
}

MDO id_table(compare), const_table(compare);
vector<pair<int, int>> fip;
string current_line = "";
int line_count = 0, line_pos = 0;

string known_atoms[] {
	"",
	"",
	"include",
	"int",
	"float",
	"if",
	"else",
	"while",
	"return",
	"!",
	"&",
	"*",
	"+",
	"-",
	"/",
	"<",
	"=",
	">",
	"?",
	"^",
	"|",
	"~",
	"==",
	"!=",
	"<=",
	">=",
	"<<",
	">>",
	"&=",
	"|=",
	"^=",
	"+=",
	"-=",
	"++",
	"--",
	"&&",
	"||",
	"#",
	"()",
	",",
	";",
	"[",
	"]",
	"{",
	"}"
};

enum STATES {
	S_START = 0,
	S_DEAD,
	S_NR,
	S_ID,
	S_STR,
	S_STR_END,
	S_SEP,
	S_OP,
	S_FLOAT
};

//typedef unsigned short (*Action)(string &token_buffer);

//map<pair<unsigned short, unsigned short>, Action> action_map;

unsigned short translation_array[] =
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 7, 5, 6, 2, 2, 7, 2, 6, 6, 7, 7, 6, 7,
	8, 7, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 6, 7, 7, 7, 7, 2, 4, 4, 4, 4,
	4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 6,
	2, 6, 7, 4, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
	4, 4, 4, 4, 4, 4, 4, 4, 6, 7, 6, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0
};

unsigned short state_table[][8] =
{
/* 				1 			2 			3 			4 			5 			6 			7 			8 */
/*  			whitespace 	printable 	digit 		letter 		quote 		separator 	operator 	dot */
/* START */		{S_START, 	S_DEAD, 	S_NR, 		S_ID, 		S_STR, 		S_SEP, 		S_OP, 		S_DEAD},
/* DEAD  */		{S_DEAD, 	S_DEAD, 	S_DEAD, 	S_DEAD, 	S_DEAD, 	S_DEAD, 	S_DEAD, 	S_DEAD},
/* NR */ 		{S_START, 	S_DEAD, 	S_NR, 		S_DEAD, 	S_DEAD, 	S_START, 	S_START, 	S_FLOAT},
/* ID */ 		{S_START, 	S_DEAD, 	S_DEAD, 	S_ID, 		S_DEAD, 	S_START, 	S_START, 	S_ID},
/* STR */ 		{S_STR, 	S_STR, 		S_STR, 		S_STR, 		S_STR_END, 	S_STR, 		S_STR, 		S_STR},
/* STR_END */ 	{S_START, 	S_START, 	S_START, 	S_START, 	S_START, 	S_START, 	S_START, 	S_START},
/* SEP */ 		{S_START, 	S_START, 	S_START, 	S_START, 	S_START, 	S_START, 	S_START, 	S_DEAD},
/* OP */ 		{S_START, 	S_START, 	S_START, 	S_START, 	S_START, 	S_START, 	S_OP, 		S_DEAD},
/* FLOAT */ 	{S_START, 	S_DEAD, 	S_FLOAT, 	S_DEAD, 	S_DEAD, 	S_START, 	S_START, 	S_DEAD}
};

string keywords[] =
{
	"include",
	"int",
	"float",
	"if",
	"else",
	"while",
	"return"
};

string multichar_operators[] =
{
	"==",
	"!=",
	"<=",
	">=",
	"<<",
	">>",
	"&=",
	"|=",
	"^=",
	"+=",
	"-=",
	"++",
	"--",
	"&&",
	"||"
};

/*
void initializeActionMap() {
	action_map[pair<
}
*/

void addKnownAtom(string buffer) {
	int length = sizeof(known_atoms) / sizeof(known_atoms[0]);

	for(int i = 2; i < length; i++) {
		if(known_atoms[i] == buffer) {
			fip.push_back(make_pair(i, -1));
		}
	}
}

void addSymbol(string buffer, MDO &table, int atom_code) {
	vector<int> found = table.cauta(buffer);
	int symbol_code = -1;

	if(found.size()) {
		symbol_code = found[0];
	} else {
		symbol_code = table.dim();
		table.adauga(buffer, table.dim());
	}

	fip.push_back(make_pair(atom_code, symbol_code));
}

void signalError(string buffer) {
	cout << "\n\033[1mInvalid sequence:\033[0m\n"
		<< "\033[1;31m" << buffer << "\033[0m"
		<< "\n\n";
	/*
	cout << "    " << line_count << ':' << line_pos - buffer.size() + 1 << " | "
		<< current_line.substr(0, line_pos - buffer.size())
		<< "\033[1;31m" << current_line.substr(line_pos - buffer.size(), buffer.size()) << "\033[0m"
		<< current_line.substr(line_pos)
		<< "\n\n";
	*/
}

void signalInvalidCharacter(char character) {
	cout << "Invalid character: " << (int)character << ' ' << line_pos << endl;
}

void registerNumber(string buffer) {
	cout << "Number: " << buffer << endl;
	addSymbol(buffer, const_table, CONST_CODE);
}

void registerIdentifier(string buffer) {
	int keyword_count = sizeof(keywords) / sizeof(keywords[0]);

	for(int i = 0; i < keyword_count; i++) {
		if(buffer == keywords[i]) {
			cout << "Keyword: " << buffer << endl;
			addKnownAtom(buffer);
			return;
		}
	}

	if(buffer.size() > MAX_ID_SIZE) {
		//cout << "Identifier too long: " << buffer << endl;
		signalError(buffer);
		return;
	}

	cout << "Identifier: " << buffer << endl;
	addSymbol(buffer, id_table, ID_CODE);
}

void registerString(string buffer) {
	cout << "String: " << buffer << endl;
	addSymbol(buffer, const_table, CONST_CODE);
}

void registerSeparator(string buffer) {
	addKnownAtom(buffer);
}

void registerOperator(string buffer) {
	if(buffer.size() == 1) {
		cout << "Operator: " << buffer << endl;
		addKnownAtom(buffer);
	} else {
		int operator_count = sizeof(multichar_operators) / sizeof(multichar_operators[0]);

		for(int i = 0; i < operator_count; i++) {
			if(buffer == multichar_operators[i]) {
				cout << "Operator: " << buffer << endl;
				addKnownAtom(buffer);
				return;
			}
		}

		signalError(buffer);
	}
}

void registerFloat(string buffer) {
	cout << "Float: " << buffer << endl;
	addSymbol(buffer, const_table, CONST_CODE);
}

void handleTokenEnd(string buffer, unsigned short state) {
	switch(state) {
		case S_NR:
			registerNumber(buffer);
			break;
		case S_ID:
			registerIdentifier(buffer);
			break;
		case S_STR_END:
			registerString(buffer);
			break;
		case S_SEP:
			registerSeparator(buffer);
			break;
		case S_OP:
			registerOperator(buffer);
			break;
		case S_FLOAT:
			registerFloat(buffer);
			break;
		default:
			break;
	}
}

char getNextChar(ifstream &input) {
	line_pos++;

	if(line_pos >= current_line.size()) {
		getline(input, current_line);
		while(!input.eof() && !current_line.size()) {
			getline(input, current_line);
		}

		if(input.eof()) {
			return 0;
		}

		line_pos = 0;
		line_count++;
	}

	return current_line[line_pos];
}

void tokenizer(ifstream &input) {
	string token_buffer = "";
	char current_char;
	int input_pos = 0;
	unsigned short previous_state = S_START,
				   current_state = S_START,
				   input_code;

	current_char = getNextChar(input);

	while(!(input.eof())) {
		input_code = translation_array[current_char];

		if(input_code == 0) {
			signalInvalidCharacter(current_char);
			current_state = S_START;
		} else {
			input_code--;
			current_state = state_table[current_state][input_code];
		}

		//cout << current_char << ' ' << current_state << '\n';

		if(current_state == S_START && token_buffer != "") {
			handleTokenEnd(token_buffer, previous_state);
			token_buffer = "";
		} else if(current_state == S_DEAD) {
			token_buffer += current_char;
			signalError(token_buffer);
			token_buffer = "";
			current_state = S_START;

			current_char = getNextChar(input);
		} else {
			if(current_state != S_START) {
				token_buffer += current_char;
			}

			current_char = getNextChar(input);
		}

		previous_state = current_state;
	}

	if(token_buffer != "") {
		handleTokenEnd(token_buffer, previous_state);
	} else if(current_state == S_DEAD) {
		token_buffer += current_char;
		signalError(token_buffer);
		token_buffer = "";
		current_state = S_START;
	}
}

void printTables() {
	cout << "\n# ID table:\n";

	vector<string> keys = id_table.multimeaCheilor();

	for(int i = 0; i < keys.size(); i++) {
		cout << keys[i] << ' ' << id_table.cauta(keys[i])[0] << '\n';
	}

	cout << "\n# Constant table:\n";

	keys = const_table.multimeaCheilor();

	for(int i = 0; i < keys.size(); i++) {
		cout << keys[i] << ' ' << const_table.cauta(keys[i])[0] << '\n';
	}

	cout << "\n# FIP:\n";

	for(int i = 0; i < fip.size(); i++) {
		cout << fip[i].first << ' ' << fip[i].second << '\n';
	}
}

int main(int argc, char** argv) {
	if(argc < 2) {
		cout << "No input file specified\n";
		return 1;
	}

	ifstream input_file(argv[1]);

    FlexLexer* lexer = new yyFlexLexer;
    while(lexer->yylex(input_file, cout) != 0);

	//tokenizer(input_file);

	printTables();

	return 0;
}
