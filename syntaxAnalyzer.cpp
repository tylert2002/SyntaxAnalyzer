#include <iostream>
#include <sstream>
#include <stack>
#include <string>
#include <vector>
#include <stdexcept>

using namespace std;

vector<string> inputs = {
    "id + id + id",
    "( id - id * id )",
    "id * id / id * id",
    "( id - id ) / ( id + id )",
    "( id + ( id / id * id / id ) / id )",
    "( id + id",
    "id id id",
    "- id + id / id )",
    "( id ( ) id )",
    "( id * id * id / id ) )"
};

string action_table[16][8] = {
    {"S5", "", "", "", "", "S4", "", ""},         // State 0
    {"", "S6", "S7", "", "", "", "", "Acc"},      // State 1
    {"", "R3", "R3", "S8", "S9", "", "R3", "R3"},  // State 2
    {"", "R6", "R6", "R6", "R6", "", "R6", "R6"},  // State 3
    {"S5", "", "", "", "", "S4", "", ""},         // State 4
    {"", "R8", "R8", "R8", "R8", "", "R8", "R8"},  // State 5
    {"S5", "", "", "", "", "S4", "", ""},         // State 6
    {"S5", "", "", "", "", "S4", "", ""},         // State 7
    {"S5", "", "", "", "", "S4", "", ""},         // State 8
    {"S5", "", "", "", "", "S4", "", ""},         // State 9
    {"", "S6", "S7", "", "", "", "S15", ""},      // State 10
    {"", "R1", "R1", "S8", "S9", "", "R1", "R1"},  // State 11
    {"", "R2", "R2", "S8", "S9", "", "R2", "R2"},  // State 12
    {"", "R4", "R4", "R4", "R4", "", "R4", "R4"},  // State 13
    {"", "R5", "R5", "R5", "R5", "", "R5", "R5"},  // State 14
    {"", "R7", "R7", "R7", "R7", "", "R7", "R7"}   // State 15
};

int nonterminal_state_table[16][3] = {
    {1, 2, 3},    // State 0
    {-1, -1, -1}, // State 1
    {-1, -1, -1}, // State 2
    {-1, -1, -1}, // State 3
    {10, 2, 3},   // State 4
    {-1, -1, -1}, // State 5
    {-1, 11, 3},  // State 6
    {-1, 12, 3},  // State 7
    {-1, -1, 13}, // State 8
    {-1, -1, 14}, // State 9
    {-1, -1, -1}, // State 10
    {-1, -1, -1}, // State 11
    {-1, -1, -1}, // State 12
    {-1, -1, -1}, // State 13
    {-1, -1, -1}, // State 14
    {-1, -1, -1}  // State 15
};

void print_cst(string cst) {
    stringstream ss(cst);
    string line;
    while (getline(ss, line)) {
        cout << line << endl;
    }
}

string indent_subtree(const string& subtree, int spaces = 3) {
    stringstream ss(subtree);
    string line, result;
    string prefix(spaces, ' ');
    while (getline(ss, line)) {
        result += prefix + line + "\n";
    }
    return result;
}

string wrap_with_nonterminal(const string& label, const string& content) {
    return label + "\n" + indent_subtree(content);
}

int get_token(string& token) {
    if (token == "id") return 0;
    if (token == "+") return 1;
    if (token == "-") return 2;
    if (token == "*") return 3;
    if (token == "/") return 4;
    if (token == "(") return 5;
    if (token == ")") return 6;
    if (token == "$") return 7;
    throw runtime_error("Invalid token: " + token);
}

int main() {
    for (string input : inputs) {
        cout << "\nProcessing: " << input << endl;

        try {
            istringstream iss(input);
            string token;
            vector<string> tokens;

            while (iss >> token) {
                if (token == "id" || token == "+" || token == "-" || token == "*" ||
                    token == "/" || token == "(" || token == ")") {
                    tokens.push_back(token);
                } else {
                    throw runtime_error("Error: Invalid token in string: " + token);
                }
            }
            tokens.push_back("$");

            stack<int> state_stack;
            stack<string> symbol_stack;
            state_stack.push(0);
            size_t token_index = 0;

            while (true) {
                int state = state_stack.top();
                string current_token = tokens[token_index];
                int token_pos = get_token(current_token);
                string action = action_table[state][token_pos];

                if (action.empty()) {
                    throw runtime_error("Syntax Error: unexpected token " + current_token + " at state " + to_string(state));
                }

                if (action == "Acc") {
                    if (symbol_stack.size() != 1)
                        throw runtime_error("Syntax error: incorrect final state");
                    cout << "Concrete Syntax Tree:\n";
                    print_cst(symbol_stack.top());
                    break;
                }

                if (action[0] == 'S') {
                    int new_state = stoi(action.substr(1));
                    symbol_stack.push(current_token);
                    state_stack.push(new_state);
                    token_index++;
                }

                if (action[0] == 'R') {
                    int reduce = stoi(action.substr(1));
                    string another_symbol, nonterminal;

                    switch (reduce) {
                        case 1: { // E -> E + T
                            string T = symbol_stack.top(); symbol_stack.pop(); state_stack.pop();
                            symbol_stack.pop(); state_stack.pop(); // +
                            string E = symbol_stack.top(); symbol_stack.pop(); state_stack.pop();
                            another_symbol = wrap_with_nonterminal("E", "+\n" + indent_subtree(E) + indent_subtree(T));
                            nonterminal = "E"; break;
                        }
                        case 2: { // E -> E - T
                            string T = symbol_stack.top(); symbol_stack.pop(); state_stack.pop();
                            symbol_stack.pop(); state_stack.pop(); // -
                            string E = symbol_stack.top(); symbol_stack.pop(); state_stack.pop();
                            another_symbol = wrap_with_nonterminal("E", "-\n" + indent_subtree(E) + indent_subtree(T));
                            nonterminal = "E"; break;
                        }
                        case 3: {
                            string T = symbol_stack.top(); symbol_stack.pop(); state_stack.pop();
                            another_symbol = wrap_with_nonterminal("E", T);
                            nonterminal = "E"; break;
                        }
                        case 4: { // T -> T * F
                            string F = symbol_stack.top(); symbol_stack.pop(); state_stack.pop();
                            symbol_stack.pop(); state_stack.pop(); // *
                            string T = symbol_stack.top(); symbol_stack.pop(); state_stack.pop();
                            another_symbol = wrap_with_nonterminal("T", "*\n" + indent_subtree(T) + indent_subtree(F));
                            nonterminal = "T"; break;
                        }
                        case 5: { // T -> T / F
                            string F = symbol_stack.top(); symbol_stack.pop(); state_stack.pop();
                            symbol_stack.pop(); state_stack.pop(); // /
                            string T = symbol_stack.top(); symbol_stack.pop(); state_stack.pop();
                            another_symbol = wrap_with_nonterminal("T", "/\n" + indent_subtree(T) + indent_subtree(F));
                            nonterminal = "T"; break;
                        }
                        case 6: {
                            string F = symbol_stack.top(); symbol_stack.pop(); state_stack.pop();
                            another_symbol = wrap_with_nonterminal("T", F);
                            nonterminal = "T"; break;
                        }
                        case 7: { // F -> ( E )
                            symbol_stack.pop(); state_stack.pop(); // )
                            string E = symbol_stack.top(); symbol_stack.pop(); state_stack.pop();
                            symbol_stack.pop(); state_stack.pop(); // (
                            another_symbol = wrap_with_nonterminal("F", "()\n" + indent_subtree(E));
                            nonterminal = "F"; break;
                        }
                        case 8: {
                            string id = symbol_stack.top(); symbol_stack.pop(); state_stack.pop();
                            another_symbol = wrap_with_nonterminal("F", id);
                            nonterminal = "F"; break;
                        }
                    }

                    symbol_stack.push(another_symbol);
                    int current_state = state_stack.top();
                    int goto_col = (nonterminal == "E") ? 0 : (nonterminal == "T") ? 1 : 2;
                    int next_state = nonterminal_state_table[current_state][goto_col];
                    if (next_state == -1)
                        throw runtime_error("Goto error for nonterminal " + nonterminal);
                    state_stack.push(next_state);
                }
            }
        } catch (const exception& e) {
            cout << e.what() << endl;
        }
    }

    return 0;
}
