#include <iostream>
#include <fstream>
//#include <map>

#define MAX_ID_SIZE 250

enum STATES {
	S_START = 0,
	S_DEAD,
	S_NR,
	S_ID,
	S_STR,
	S_STR_END,
	S_SEP,
	S_OP
};

//typedef unsigned short (*Action)(string &token_buffer);

using namespace std;

//map<pair<unsigned short, unsigned short>, Action> action_map;

unsigned short translation_array[] =
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 7, 5, 6, 2, 2, 7, 2, 6, 6, 7, 7, 6, 7,
	4, 7, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 6, 7, 7, 7, 7, 2, 4, 4, 4, 4,
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

unsigned short state_table[][7] = 
{
/* 				1 			2 			3 			4 			5 			6 			7 */
/*  			whitespace 	printable 	digit 		letter 		quote 		separator 	operator */
/* START */		{S_START, 	S_DEAD, 	S_NR, 		S_ID, 		S_STR, 		S_SEP, 		S_OP},
/* DEAD  */		{S_DEAD, 	S_DEAD, 	S_DEAD, 	S_DEAD, 	S_DEAD, 	S_DEAD, 	S_DEAD},
/* NR */ 		{S_START, 	S_DEAD, 	S_NR, 		S_DEAD, 	S_DEAD, 	S_START, 	S_START},
/* ID */ 		{S_START, 	S_DEAD, 	S_DEAD, 	S_ID, 		S_DEAD, 	S_START, 	S_START},
/* STR */ 		{S_STR, 	S_STR, 		S_STR, 		S_STR, 		S_STR_END, 	S_STR, 		S_STR},
/* STR_END */ 	{S_START, 	S_START, 	S_START, 	S_START, 	S_START, 	S_START, 	S_START},
/* SEP */ 		{S_START, 	S_START, 	S_START, 	S_START, 	S_START, 	S_START, 	S_START},
/* OP */ 		{S_START, 	S_START, 	S_START, 	S_START, 	S_START, 	S_START, 	S_OP}
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

/*
void initializeActionMap() {
	action_map[pair<
}
*/

void registerNumber(string buffer) {
	cout << "Number: " << buffer << endl;
}

void registerIdentifier(string buffer) {
	int keyword_count = sizeof(keywords) / sizeof(keywords[0]);

	for(int i = 0; i < keyword_count; i++) {
		if(buffer == keywords[i]) {
			cout << "Keyword: " << buffer << endl;
			return;
		}
	}

	if(buffer.size() > MAX_ID_SIZE) {
		cout << "Identifier too long: " << buffer << endl;
		return;
	}

	cout << "Identifier: " << buffer << endl;
}

void registerString(string buffer) {
	cout << "String: " << buffer << endl;
}

void registerSeparator(string buffer) {
	cout << "Separator: " << buffer << endl;
}

void registerOperator(string buffer) {
	cout << "Operator: " << buffer << endl;
}

void signalError(string buffer) {
	cout << "Invalid sequence: " << buffer << endl;
}

void signalInvalidCharacter(char character) {
	cout << "Invalid character: " << character << endl;
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
		default:
			break;
	}
}

char getNextChar(ifstream &input) {
	char next_char;

	input >> noskipws >> next_char;

	return next_char;
}

void tokenizer(ifstream &input) {
	string token_buffer = "";
	char current_char;
	int input_pos = 0;
	unsigned short previous_state = S_START,
				   current_state = S_START,
				   input_code;

	input >> current_char;

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
		signalError(token_buffer);
		token_buffer = "";
		current_state = S_START;
	}
}

int main(int argc, char** argv) {
	if(argc < 2) {
		cout << "No input file specified\n";
		return 1;
	}

	ifstream input_file(argv[1]);

	tokenizer(input_file);

	return 0;
}
