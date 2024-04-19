#include "Token.h"
#include "Lexer.h"
#include "Helper.h"


// private function to open file?
// isTokenIdentifier function
// isTokenNumber function
// isTokenBinaryOperator
// Tokenize should essentially just call the above functions
// followed by running through the entirety of the file and 
// outputting it into the terminal such that we're free
// from ugly code in main

int Lexer::isTokenIdentifier(char c, FILE* fp, std::string sb) {
	Token tok;
	Helper help;
	// Is identifier? [a-zA-Z][a-zA-Z0-9]*
	if (help.is_identifier(c))
	{
		sb.clear();
		do
		{
			sb.push_back(c);
			c = fgetc(fp);
		} while (isalnum(c) || c == '_');
		
		if (!feof(fp)) ungetc(c, fp);
		
		if (sb == "behavior")
		{
			return tok.TOKEN_BEHAVIOR;
		}
		else if(sb == "action") {
			return tok.TOKEN_ACTION;
		} else {
			return tok.TOKEN_ID;
		}
	}
	return 0;
}

int Lexer::isTokenNumber(char c, FILE* fp) {
	Token tok;
	double numVal;
	Helper help;
	int is_ok = EXIT_FAILURE;
	if(help.is_digit(c) || c == '.') {
		std::string NumStr;
		do
		{
			NumStr += c;
			c = getchar();
		} while (help.is_digit(c) || c == '.');
		numVal = strtod(NumStr.c_str(), nullptr);
		return tok.TOKEN_NUM;
	}
	return is_ok = EXIT_FAILURE;
}

int Lexer::isTokenBinOp(char c, FILE* fp) {
	int is_ok = EXIT_FAILURE;
	Token tok;

	switch (c)
	{
	case '+':
		return tok.TOKEN_PLUS;
		break;
	case '-':
		return tok.TOKEN_MINUS;
		break;
	case '*':
		return tok.TOKEN_TIMES;
		break;
	case '/':
		return tok.TOKEN_DIVIDE;
		break;
	case '=':
		return tok.TOKEN_EQUAL;
		break;
	case '<':
		return tok.TOKEN_LESSTHAN;
		break;
	case '>':
		return tok.TOKEN_GREATERTHAN;
		break;
	default:
		break;
	}
	return is_ok = EXIT_FAILURE;
}


// using fopen rn, wtf am i doing
int Lexer::Tokenize(const char *fileName) {
	int c; // required for EOF
	int is_ok = EXIT_FAILURE;
	std::string sb; // string buffer
	Token tok;
	
	FILE * fp = fopen(fileName, "r");
    if (!fp) {
        perror ("Error opening provided file");
		return is_ok = EXIT_FAILURE;
	}
	else {
		while ((c = fgetc(fp)) != EOF)
		{
			isTokenIdentifier(c, fp, sb);
			isTokenNumber(c, fp);
			isTokenBinOp(c, fp);
			if (ferror(fp))
			puts("I/O error when reading");
			else if (feof(fp)) {
				puts("End of file is reached successfully");
				is_ok = EXIT_SUCCESS;
			}
		}
	}

	return is_ok = EXIT_FAILURE;
}