# SyntaxAnalyzer

To start, we have 
- a vector of all the inputs as strings
- String action table
- Int nonterminal state table
For each test case, we turn the input into a vector of tokens with $ being the last symbol inserted 
- We do this by turning the string into a string stream and pushing all valid tokens into the tokens vector 
After that, we declare
- stack<int> state_stack; a stack of ints to represent the parsing states, and we push 0 into it
- stack<string> symbol_stack; holds the symbols or subtrees used in parsing
- state_stack.push(0); initializes the parser
- size_t token_index = 0; to iterate through the tokens vector 
Loop:
- Declare
  - int state = state_stack.top(); represents row in action_table
  - string current_token = tokens[token_index]; gets the current input token
  - int token_pos = get_token(current_token); represents column in action_table
  - string action = action_table[state][token_pos]; gets the parser action baked on state and token
If action = "Acc"
- Print the parse tree
If the action starts with the letter S (shift)
- Push the next token into the stack to be parsed 
- Extract the number after S using stoi(action.substr(1)) to get the next state
- Push the current token into the symbol stack
- Push the next state into the state_stack
- Increment token_index to move to the next input token
If the action starts with the letter R (reduce), look at the number following R
1: E -> E + T
2: E -> E - T
3: E -> T
4: T -> T * F
5: T -> T / F
6: T -> F
7: F -> ( E )
8: F -> id
(for all of these, orient the next steps in the parse tree it creates and push it into the symbol_stack to be output)
Set the next state to loop through 
Transformed Grammar: 
E -> TQ 
Q -> +TQ | -TQ | ε
T -> FR 
R -> *FR | /FR | ε 
F -> ( E ) | id
