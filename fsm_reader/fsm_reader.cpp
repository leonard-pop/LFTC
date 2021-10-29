#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <set>

#define INVALID_INPUT -1

using namespace std;

vector<int> translation_array(256, INVALID_INPUT);
set<int> final_states;
int start_state;

map<string, int> state_map;
bool alphabet_read = false, states_read = false, started_transitions = false;
vector<vector<int>> transition_table;

enum READER_STATE {
	ALPHABET,
	STATES,
	TRANSITIONS
};

string extended_whitespace = " \t\n\r";
string line_whitespace = " \t";

void signalError(const string &message) {
	cout << "Error: " << message << '\n';
}

void stripString(string &str) {
	while(str.size() && extended_whitespace.find(str[-1]) != string::npos) {
		str.pop_back();
	}
}

void skipCharacters(int *pos, const string &str, const string &skip_list, int increment) {
	while(*pos >= 0 && *pos < str.size() && skip_list.find(str[*pos]) != string::npos) {
		(*pos) += increment;
	}
}

char getNextChar(string line, int *pos) {
	(*pos)++;
	if(*pos < line.size()) {
		return line[*pos];
	} else {
		return 0;
	}
	
	/*
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
	*/
}

void signalInvalidCharacter(char c) {
	cout << "Invalid character: " << c << '\n';
}

int fsm(string &line) {
	string token_buffer = "";
	int pos = 0;
	char current_char;
	int previous_state = start_state,
	   current_state = start_state,
	   input_code, last_accepting = -1;

	while(pos < line.size()) {
		current_char = line[pos];
		input_code = translation_array[current_char];

		//cout << current_state << " - " << input_code << " - ";

		if(input_code == INVALID_INPUT) {
			return last_accepting;
		} else {
			current_state = transition_table[current_state][input_code];
		}

		//cout << current_state << '\n';

		//cout << current_char << ' ' << current_state << '\n';

		/*
		if(current_state == S_START && token_buffer != "") {
			handleTokenEnd(token_buffer, previous_state);
			token_buffer = "";
		} else if(current_state == S_DEAD) {
			token_buffer += current_char;
			signalError(token_buffer);
			token_buffer = "";
			current_state = S_START;

			current_char = getNextChar(input);
		} else {*/


		if(final_states.count(current_state)) {
			last_accepting = pos;
		}

		pos++;
	}

	return last_accepting;
}

int handleAlphabetLine(string &line) {
	alphabet_read = true;
	if(line[0] != '[') {
		signalError("Invalid alphabet line");
		return 1;
	}


	int pos = line.size() - 1,
		code;
	char last_char = 0;

	while(pos >= 0 && isdigit(line[pos])) {
		pos--;
	}
	code = atoi(line.c_str() + pos + 1);

	skipCharacters(&pos, line, line_whitespace, -1);
	if(line[pos] != '=') {
		signalError("Invalid alphabet line");
		return 1;
	}
	pos--;

	skipCharacters(&pos, line, line_whitespace, -1);

	if(line[pos] != ']') {
		signalError("Invalid alphabet line");
		return 1;
	}
	pos--;

	char left, right;

	while(pos >= 0 && line[pos] != ']') {
		if(line[pos] == '-' && line[pos + 1] != ']') {
			left = line[pos - 1];
			right = line[pos + 1];

			if(left == '[' || left > right) {
				signalError("Invalid range");
				return 1;
			}

			for(char c = left; c <= right; c++) {
				translation_array[c] = code;
			}
		} else {
			translation_array[line[pos]] = code;
		}

		pos--;
	}

	return 0;
}

int addState(string state_name) {
	if(!state_map.count(state_name)) {
		state_map[state_name] = state_map.size();
	}

	return state_map[state_name];
}

int handleStatesLine(string &line) {
	states_read = true;
	int pos = line.find('-');
	vector<int> line_states;
	string name_buffer;

	if(pos == string::npos) {
		signalError("Invalid states line");
		return 1;
	}

	pos++;
	while(pos < line.size()) {
		skipCharacters(&pos, line, line_whitespace, 1);

		name_buffer = "";
		while(pos < line.size() && isalnum(line[pos])) {
			name_buffer += line[pos];
			pos++;
		}

		line_states.push_back(addState(name_buffer));
		
		skipCharacters(&pos, line, line_whitespace, 1);

		pos++;
	}

	pos = 0;
	while(pos < line.size() && isalpha(line[pos])) {
		pos++;
	}

	if(line.substr(0, pos) == "start") {
		if(line_states.size() != 1) {
			signalError("There must be only one start state");
			return 1;
		}

		start_state = line_states[0];
	} else if(line.substr(0, pos) == "final") {
		for(int state: line_states) {
			final_states.insert(state);
		}
	} else if(line.substr(0, pos) != "extra") {
		signalError("Invalid state type");
		return 1;
	}

	return 0;
}

int handleTransitionsLine(string &line) {
	if(!states_read || !alphabet_read) {
		signalError("Transitions must come last");
		return 1;
	}
	if(!started_transitions) {
		transition_table.resize(state_map.size());
		for(int i = 0; i < transition_table.size(); i++) {
			transition_table[i].resize(256, -1);
		}

		started_transitions = true;
	}

	int left_state, right_state, input_code;

	string buffer = "";
	int pos = 0;

	while(pos < line.size() && isalnum(line[pos])) {
		buffer += line[pos];
		pos++;
	}

	left_state = state_map[buffer];

	skipCharacters(&pos, line, line_whitespace, 1);
	pos++;
	skipCharacters(&pos, line, line_whitespace, 1);

	buffer = "";
	while(pos < line.size() && isdigit(line[pos])) {
		buffer += line[pos];
		pos++;
	}

	input_code = atoi(buffer.c_str());

	skipCharacters(&pos, line, line_whitespace, 1);
	pos++;
	skipCharacters(&pos, line, line_whitespace, 1);

	buffer = "";
	while(pos < line.size() && isalnum(line[pos])) {
		buffer += line[pos];
		pos++;
	}

	right_state = state_map[buffer];
	//cout << "Right state: \"" << buffer << "\" = " << right_state << '\n';

	transition_table[left_state][input_code] = right_state;

	return 0;
}

void printAlphabet() {
	for(int i = 0; i < 256; i++) {
		if(translation_array[i] != INVALID_INPUT) {
			cout << (char) i << " = " << translation_array[i] << '\n';
		}
	}
}

void printStates() {
	for(map<string,int>::iterator it = state_map.begin(); it != state_map.end(); it++) {
		cout << it->first << " - " << it->second << '\n';
	}
}

void printFinalStates() {
	for(int state: final_states) {
		cout << state << ' ';
	}
	cout << '\n';
}

void printTransitions() {
	for(int i = 0; i < transition_table.size(); i++) {
		for(int j = 0; j < 256; j++) {
			if(transition_table[i][j] != -1) {
				cout << i << " - " << transition_table[i][j] << " - " << j << '\n';
			}
		}
	}
}

int readFile(string file_name) {
	ifstream input(file_name);
	string line = "";
	int state = 0;

	getline(input, line);

	while(!input.eof()) {
		stripString(line);

		if(line.size() != 0) {
			if(line[0] == '.') {
				if(line == ".alphabet") {
					state = ALPHABET;
				} else if(line == ".alphabet") {
					state = ALPHABET;
				} else if(line == ".states") {
					state = STATES;
				} else if(line == ".transitions") {
					state = TRANSITIONS;
				} else {
					signalError("Invalid state");
					return 1;
				}
			} else {
				switch(state) {
					case ALPHABET:
						handleAlphabetLine(line);
						break;
					case STATES:
						handleStatesLine(line);
						break;
					case TRANSITIONS:
						handleTransitionsLine(line);
						break;
					default:
						signalError("No section name specified");
						return 1;
				}
			}
		}

		getline(input, line);
	}
	
	return 0;
}

void checkIfAccepted() {
	string line;
	int last_accepting;

	getline(cin, line);
	cout << "Input string: ";
	getline(cin, line);
	stripString(line);

	last_accepting = fsm(line);
	//cout << "last accepting " << last_accepting << '\n';

	if(last_accepting == line.size() - 1) {
		cout << "Accepted\n";
	} else {
		cout << "Rejected\n";
	}
}

void printLongestAcceptingPrefix() {
	string line;
	int last_accepting;

	getline(cin, line);
	cout << "Input string: ";
	getline(cin, line);
	stripString(line);

	last_accepting = fsm(line);

	if(last_accepting != -1) {
		cout << "Accepted prefix: " + line.substr(0, last_accepting + 1) << '\n';
	} else {
		cout << "Entire string rejected\n";
	}
}

int main(int argc, char** argv) {
	if(argc < 2) {
		return 1;
	}
	bool running = true;
	string command_string;
	int command;

	if(!readFile(argv[1])) {
		while(running) {
			cout << "--------------------\n"
				<< "Actions:\n"
				<< " 1 - print alphabet\n"
				<< " 2 - print all states\n"
				<< " 3 - print final states\n"
				<< " 4 - print transitions\n"
				<< " 5 - check if string is accepted\n"
				<< " 6 - get longest accepting string\n"
				<< " 0 - exit\n"
				<< "\nEnter option: ";
			cin >> command;

			switch(command) {
				case 0:
					running = false;
					break;
				case 1:
					printAlphabet();
					break;
				case 2:
					printStates();
					break;
				case 3:
					printFinalStates();
					break;
				case 4:
					printTransitions();
					break;
				case 5:
					checkIfAccepted();
					break;
				case 6:
					printLongestAcceptingPrefix();
					break;
				default:
					cout << "Invalid command\n";
			}

		}

		return 0;
	}

	return 1;
}
