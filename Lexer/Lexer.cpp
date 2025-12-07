#include "Tokens/Token.h"
#include <optional>
#include <fstream>
#include <cstring>
#include <iostream>
#include <map>
#include <unordered_map>
#include <vector>
#include "Lexer.h"

bool Lexer::setInputFile(const std::string& filename) {
    inputFile.open(filename);
    if (!inputFile.is_open()) {
        return false;
    }
    usingFile = true;
    LastChar = ' ';
    return true;
}

int Lexer::getChar() {
    if (usingFile && inputFile.is_open()) {
        if (inputFile.eof()) {
            return EOF;
        }
        int ch = inputFile.get();
        if (inputFile.eof()) {
            return EOF;
        }
        return ch;
    }
    return getchar();
}

// getToken - Return the next token from input.
int Lexer::getToken() {

    // Skip any whitespace.
    while (isspace(LastChar))
        LastChar = getChar();

	if (isalpha(LastChar) || LastChar == '_') { // identifier: [a-zA-Z_][a-zA-Z0-9_]*
		IdentifierStr = LastChar;
		while (isalnum((LastChar = getChar())) || LastChar == '_') {
			IdentifierStr += LastChar;
		}

		// Check for keywords using a map for cleaner code
		static const std::unordered_map<std::string, Token::Type> keywordMap = {
			{"def", Token::Type::TOKEN_DEF},
			{"extern", Token::Type::TOKEN_EXTERN},
			{"behavior", Token::Type::TOKEN_BEHAVIOR},
			{"action", Token::Type::TOKEN_ACTION},
			{"object", Token::Type::TOKEN_OBJECT},
			{"inherits", Token::Type::TOKEN_INHERITS},
			{"if", Token::Type::TOKEN_IF},
			{"else", Token::Type::TOKEN_ELSE},
			{"return", Token::Type::TOKEN_RETURN},
			{"int", Token::Type::TOKEN_INT},
			{"string", Token::Type::TOKEN_STRING},
			{"obj", Token::Type::TOKEN_OBJ},
			{"Console", Token::Type::TOKEN_CONSOLE},
			{"null", Token::Type::TOKEN_NULL}
		};

		auto it = keywordMap.find(IdentifierStr);
		if (it != keywordMap.end()) {
			return it->second;
		}
		return Token::Type::TOKEN_ID;
	}

	// String literals: "..." or '...'
	if (LastChar == '"' || LastChar == '\'') {
		char quote = LastChar;
		StringVal = "";
		LastChar = getChar();
		while (LastChar != quote && LastChar != EOF && LastChar != '\n' && LastChar != '\r') {
			if (LastChar == '\\') {
				LastChar = getChar();
				switch (LastChar) {
					case 'n': StringVal += '\n'; break;
					case 't': StringVal += '\t'; break;
					case '\\': StringVal += '\\'; break;
					case '"': StringVal += '"'; break;
					case '\'': StringVal += '\''; break;
					default: StringVal += '\\'; StringVal += LastChar; break;
				}
			} else {
				StringVal += LastChar;
			}
			LastChar = getChar();
		}
		if (LastChar == quote) {
			LastChar = getChar();
			return Token::Type::TOKEN_STRING_LITERAL;
		} else {
			return Token::Type::TOKEN_EOF; // Unterminated string
		}
	}

    if (isdigit(LastChar) || LastChar == '.') { // Number: [0-9.]+
        std::string NumStr;
        do {
            NumStr += LastChar;
            LastChar = getChar();
        } while (isdigit(LastChar) || LastChar == '.');

        NumVal = strtod(NumStr.c_str(), nullptr);
        return Token::Type::TOKEN_NUM;
    }
	// Comments: # or //
	if (LastChar == '#') {
		// Skip single-line comments
		do {
			LastChar = getChar();
		} while (LastChar != EOF && LastChar != '\n' && LastChar != '\r');

		if (LastChar != EOF)
			return getToken();
	}
	if (LastChar == '/') {
		int NextChar = getChar();
		if (NextChar == '/') {
			// Skip // comments
			do {
				LastChar = getChar();
			} while (LastChar != EOF && LastChar != '\n' && LastChar != '\r');

			if (LastChar != EOF)
				return getToken();
		} else {
			// Not a comment, put back the second char
			if (usingFile && inputFile.is_open()) {
				inputFile.putback(NextChar);
			} else {
				ungetc(NextChar, stdin);
			}
			LastChar = '/';
		}
	}

	if (LastChar == EOF)
		return Token::Type::TOKEN_EOF;

	// Handle operators and punctuation using switch statement
	switch (LastChar) {
		case '.':
			LastChar = getChar();
			return Token::Type::TOKEN_DOT;
		
		case '=':
			LastChar = getChar();
			if (LastChar == '=') {
				LastChar = getChar();
				return Token::Type::TOKEN_EQUAL; // ==
			}
			return '='; // Single =
		
		case '!':
			LastChar = getChar();
			if (LastChar == '=') {
				LastChar = getChar();
				return Token::Type::TOKEN_NOTEQUAL; // !=
			}
			return '!';
		
		case '<':
			LastChar = getChar();
			if (LastChar == '=') {
				LastChar = getChar();
				return Token::Type::TOKEN_LESSEQUAL; // <=
			}
			return Token::Type::TOKEN_LESSTHAN; // <
		
		case '>':
			LastChar = getChar();
			if (LastChar == '=') {
				LastChar = getChar();
				return Token::Type::TOKEN_GREATEREQUAL; // >=
			}
			return Token::Type::TOKEN_GREATERTHAN; // >
		
		case '+':
			LastChar = getChar();
			return Token::Type::TOKEN_PLUS;
		
		case '-':
			LastChar = getChar();
			return Token::Type::TOKEN_MINUS;
		
		case '*':
			LastChar = getChar();
			return Token::Type::TOKEN_TIMES;
		
		case '/':
			LastChar = getChar();
			return Token::Type::TOKEN_DIVIDE;
		
		case '(':
			LastChar = getChar();
			return Token::Type::TOKEN_LEFT_PARENTHESES;
		
		case ')':
			LastChar = getChar();
			return Token::Type::TOKEN_RIGHT_PARENTHESES;
		
		case '{':
			LastChar = getChar();
			return Token::Type::TOKEN_LEFT_CURLYBRAC;
		
		case '}':
			LastChar = getChar();
			return Token::Type::TOKEN_RIGHT_CURLYBRAC;
		
		case ';':
			LastChar = getChar();
			return Token::Type::TOKEN_SEMICOLON;
		
		case ',':
			LastChar = getChar();
			return Token::Type::TOKEN_COMMA;
		
		default:
			break;
	}

	// Return the character as its ASCII value, and then read the next character
	int ThisChar = LastChar;
	LastChar = getChar();
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
	return p;
}