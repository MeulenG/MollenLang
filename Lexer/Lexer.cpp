#include "Tokens/Token.h"
#include <optional>
#include <fstream>
#include <cstring>
#include <iostream>
#include <map>
#include <unordered_map>
#include <vector>
#include "Lexer.h"


// getToken - Return the next token from standard input.
int Lexer::getToken() {
    static int LastChar = ' ';

    // Skip any whitespace.
    while (isspace(LastChar))
        LastChar = getchar();

	if (isalpha(LastChar)) { // identifier: [a-zA-Z][a-zA-Z0-9]*
		IdentifierStr = LastChar;
		while (isalnum((LastChar = getchar()))) {
			IdentifierStr += LastChar;
		}

		if (IdentifierStr == "def") {
			return Token::Type::TOKEN_DEF;
		}
		if (IdentifierStr == "extern") {
			return Token::Type::TOKEN_EXTERN;
		}
		return Token::Type::TOKEN_ID;
	}

    if (isdigit(LastChar) || LastChar == '.') { // Number: [0-9.]+
        std::string NumStr;
        do {
            NumStr += LastChar;
            LastChar = getchar();
        } while (isdigit(LastChar) || LastChar == '.');

        NumVal = strtod(NumStr.c_str(), nullptr);
        return Token::Type::TOKEN_NUM;
    }
    if (LastChar == '#') {
        // Skip comments
        do {
            LastChar = getchar();
        } while (LastChar != EOF && LastChar != '\n' && LastChar != '\r');

        if (LastChar != EOF)
            return getToken();
    }

    if (LastChar == EOF)
        return Token::Type::TOKEN_EOF;

    // Return the character as its ASCII value, and then read the next character
    int ThisChar = LastChar;
    LastChar = getchar();
    return ThisChar;
}


std::string Lexer::splitFile(const char* file) {
	std::string str("if (x == 10) { y = 100; } else { y = 200; }");
    std::unordered_map<std::string, std::string> tokenMap = {
        {"if", "KEYWORD_IF"},
        {"else", "KEYWORD_ELSE"},
        {"==", "OPERATOR_EQ"},
        {"=", "OPERATOR_ASSIGN"},
        {"(", "PAREN_OPEN"},
        {")", "PAREN_CLOSE"},
        {"{", "BRACE_OPEN"},
        {"}", "BRACE_CLOSE"},
        {";", "SEMICOLON"}
    };

    char *cstr = new char[str.length() + 1];
    std::strcpy(cstr, str.c_str());
    const char *delimiters = " (){};";

    char *p = std::strtok(cstr, delimiters);
    while (p != nullptr) {
        std::string token = p;
        if (tokenMap.find(token) != tokenMap.end()) {
            std::cout << "Token: \"" << token << "\", Type: " << tokenMap[token] << '\n';
        } else {
            // Check if it's a number
            bool isNumber = true;
            for (char c : token) {
                if (!isdigit(c)) {
                    isNumber = false;
                    break;
                }
            }
            if (isNumber) {
                std::cout << "Token: \"" << token << "\", Type: NUMBER\n";
            } else {
                std::cout << "Token: \"" << token << "\", Type: IDENTIFIER\n";
            }
        }
        p = std::strtok(nullptr, delimiters);
    }

    delete[] cstr;
	// return tokens;
}