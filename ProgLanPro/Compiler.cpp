#include<iostream>
#include<cstdlib>
#include<fstream>
#include<string>
#include<vector>
#include<stack>
#include<map>

using namespace std;

/*Class token- Used to identify each part of a statement.
 *string type- Holds the type of token.
 *string value- Holds the value of the token.
 */
class token{
public:
    string type;
    string value;
    token(string t, string v){
        type = t;
        value = v;
    };
};

//Prototypes.
vector<token> tokenize(string,int);
int tokenize_identifier(string,int,vector<token>&);
int tokenize_literal(string,int,vector<token>&,int);
void tokenize_operator(string,int,vector<token>&);
void tokenize_parenthesis(string,int,vector<token>&);
void parser(map<string,int>&,vector<token>&);
void solver(vector<token>&,int,int);
void unary(vector<token>&,int,int);

int main(){
    //Adjustable vector of statements that represents the entire code.
    vector<vector<token>> code;
    //Map to hold all of the identifiers and their corresponding values.
    map<string,int> ids;
    //Declare args to hold the input code, code as the input file, and c to count the lines.
    string args;
    ifstream lang;
    int c = 0;
    lang.open("C:\\Users\\unkno\\OneDrive\\Desktop\\ProgLanPro\\code.txt");
    //Reads in the code line by line, tokenizes the statements and adds them to the code vector.
    while(!lang.eof()){
        getline(lang,args);
        if(args.size() != 0){
            c++;
            code.push_back(tokenize(args,c));
        }
    }
//Used to track the tokens.
/*    for(int i = 0; i < code.size(); i++){
        cout << "Line " << i+1 << ":" << endl;
        for(int j = 0; j < code[i].size(); j++){
            cout << "Type:" << code[i][j].type << " Value:" << code[i][j].value << endl;
        }
        cout << endl;
    }
*/
    //Parse the code.
    for(int i = 0; i < code.size(); i++){
        parser(ids, code[i]);
    }
    //Print the identifiers and their values.
    cout << "Final Identifiers:" << endl;
    std::map<string,int>::iterator it;
    for(auto const& pair: ids){
        cout << pair.first << ": " << pair.second << endl;
    }

    return 0;
}

/*Tokenize- Main function for going through a statement to analyze it.
 *args- The current statement being worked on.
 *Returns a vector holding all the tokens for this line.
 */
vector<token> tokenize(string args, int c){
    stack<char> paren;
    vector<token> s;
    //i is used to keep track of how much of the statement has been tokenized.
    int i = 0;
    //Getting rid of white space.
    while(args[i] == ' ') i++;
    //Check that the statement begins with an identifier.
    if(isalpha(args[i]) == 0 && args[i] != '_'){
        cout << "Error(line " << c << "), Identifier must start with letter value or an underscore.";
        exit(1);
    }
    //Create a token of the identifier.
    i = tokenize_identifier(args,i,s);

    //Move through white space and check for an equal sign.
    while(args[i] == ' ') i++;
    if(args[i] != '='){
        cout << "Error(line " << c << "), missing equal sign.";
        exit(1);
    }
    token eq("Assignment", "=");
    s.push_back(eq);
    i++;

    //Creating tokens for the expression until a ';' is found.
    while(args[i] != ';'){
        while(args[i] == ' ') i++;
        if(i == args.size()){
            cout << "Error(line " << c << "), missing ';'.";
            exit(1);
        }
        if(isdigit(args[i]) != 0) i = tokenize_literal(args,i,s,c);
        else{
            if(isalpha(args[i]) != 0 || args[i] == '_') i = tokenize_identifier(args,i,s);
            else{
                if(args[i] == '+' || args[i] == '-' || args[i] == '*'){
                    tokenize_operator(args,i,s);
                    i++;
                }
                else{
                    if(args[i] == '(' || args[i] == ')'){
                        tokenize_parenthesis(args,i,s);
                        i++;
                    }
                    else{
                        cout << "Error(line " << c << "), unknown symbol in statement.";
                        exit(1);
                    }
                }
            }
        }
    }

    for(int i = 0; i < s.size(); i++){
        if(s[i].type == "Parenthesis"){
            if(s[i].value == "(") paren.push('(');
            else{
                if(paren.size() == 0){
                    cout << "Error(line " << c << "), mismatched parenthesis.";
                    exit(1);
                }
                else paren.pop();
            }
        }
    }
    if(paren.size() != 0){
        cout << "Error(line " << c << "), mismatched parenthesis.";
        exit(1);
    }

    return s;
}

//Tokenize identifiers consisting of starting with letters or an underscore.
int tokenize_identifier(string args, int i, vector<token>& s){
    string temp = "";
    temp += args[i];
    i++;
    while(isalpha(args[i]) != 0 || isdigit(args[i]) != 0 || args[i] == '_'){
        temp += args[i];
        i++;
    }
    token t("Identifier", temp);
    s.push_back(t);
    return i;
}

//Tokenize literals consisting of 0 or numbers starting with a non-zero digit.
int tokenize_literal(string args, int i, vector<token>& s, int c){
    string temp = "";
    if(args[i] == '0' && isdigit(args[i+1]) != 0){
        cout << "Error(line " << c << "), literals cannot start with 0, unless it's only 0.";
        exit(1);
    }
    while(isdigit(args[i]) != 0){
        temp += args[i];
        i++;
    }
    token t("Literal",temp);
    s.push_back(t);

    return i;
}

//Tokenize operators +, - and *.
void tokenize_operator(string args, int i, vector<token>& s){
    string temp = "";
    temp += args[i];
    token t("Operator",temp);
    s.push_back(t);
    return;
}

//Tokenize opening and closing parenthesis.
void tokenize_parenthesis(string args, int i, vector<token>& s){
    string temp = "";
    temp += args[i];
    token t("Parenthesis",temp);
    s.push_back(t);
    return;
}

//Parse each statement.
void parser(map<string,int>& ids, vector<token>& statement){
    int first, last;
    int c = 0;
    int j;

    //Check for uninitialized variables and convert identifiers to literals.
    for(int i = 2; i < statement.size(); i++){
        if(statement[i].type == "Identifier"){
            statement[i].type = "Literal";
            statement[i].value = to_string(ids.at(statement[i].value));
        }
    }

    //Check for expressions in parenthesis.
    for(int i = 2; i < statement.size(); i++){
        if(statement[i].value == "(") first = i+1;
        if(statement[i].value == ")"){
            solver(statement,first,i-1);
            statement.erase(statement.begin()+first+1);
            statement.erase(statement.begin()+first-1);
            i = 1;
        }
    }
    //Solve remaining expression and apply the new value to the identifier.
    solver(statement,2,statement.size()-1);
    ids[statement[0].value] = stoi(statement[2].value);

    return;
}

//Solve a given expression.
void solver(vector<token>& statement, int first, int last){
    int i = last-1;
    if(statement[last].type == "Operator"){
        cout << "Error, expression cannot end with an operator.";
        exit(1);
    }
    //Find and solve unary operators.
    while(i > first){
        if(statement[i].type == "Operator" && statement[i-1].type == "Operator"){
           unary(statement,i,i+1);
           last--;
           i = last;
        }
        else i--;
    }
    if(statement[first].type == "Operator"){
        unary(statement,first,first+1);
        last--;
    }
    //Solve multiplication, followed by addition and subtraction.
    for(int i = first; i < last; i++){
        if(statement[i].value == "*"){
            statement[i-1].value = to_string(stoi(statement[i-1].value) * stoi(statement[i+1].value));
            statement.erase(statement.begin()+i+1);
            statement.erase(statement.begin()+i);
            last = last - 2;
            i--;
        }
    }
    for(int i = first; i < last; i++){
        if(statement[i].type == "Operator"){
            if(statement[i].value == "+") statement[i-1].value = to_string((stoi(statement[i-1].value)) + (stoi(statement[i+1].value)));
            else statement[i-1].value = to_string(stoi(statement[i-1].value) - stoi(statement[i+1].value));
            statement.erase(statement.begin()+i+1);
            statement.erase(statement.begin()+i);
            last = last - 2;
            i--;
        }
    }

    return;
}

//Solving unary operators.
void unary(vector<token>&statement, int op, int value){
    if(statement[op].value == "*"){
        cout << "Error, misplaced *.";
        exit(1);
    }
    if(statement[op].value == "-") statement[value].value = to_string(-(stoi(statement[value].value)));
    statement.erase(statement.begin()+op);
    return;
}
