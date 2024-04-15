#include <cctype>
#include <fstream>
#include <iostream>
#include "Token.h"
#include "Lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <sys/stat.h>

int Lexer::readAndRemoveWS(int fd_in, int fd_out) {
	// local variables
	char c;
	int line = 0;
	int is_not_ok = EXIT_FAILURE;

	while (read(fd_in, &c, 1) > 0) {
        if (c != ' ' && c != '\t' && c != '\n') {
			if (createCopyFromMemory(fd_out, &c, 1) == -1)
			{
				perror("Write to copied source program failed!");
				return is_not_ok = EXIT_FAILURE;
			}
		}
		if (c == '\n')
		{
			line++;
		}
    }
	return is_not_ok;
}

int Lexer::readFileDescriptor(const char *file, const char *file2) {
	// local variables
	int is_not_ok = EXIT_FAILURE;
	
	int source_fd = open(file, O_RDWR, 0);
	if (source_fd < 0)
	{
		perror("open source");
		return is_not_ok = EXIT_FAILURE;
	}

	int target_fd = open(file2, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (target_fd < 0)
	{
		perror("open target");
		return is_not_ok = EXIT_FAILURE;
	}
	

	readAndRemoveWS(source_fd, target_fd);
	close(source_fd);
	close(target_fd);
    return is_not_ok = EXIT_FAILURE;
}

int Lexer::createCopyFromMemory(int fd, const void *buf, size_t n) {
    ssize_t bytesWritten = write(fd, buf, n);

    if (bytesWritten == -1) {
        perror("Failed to write to file");
        return EXIT_FAILURE;
    }
    if (static_cast<size_t>(bytesWritten) != n) {
        perror("Incomplete write");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}



int Lexer::Tokenize(const char *fileName) {
	int line = 1;
	int c; // required for EOF
	int is_not_ok = EXIT_FAILURE;
	std::string sb; // string buffer
	Token tok;
	
	FILE * fp = fopen(fileName, "r");
    if (!fp) {
        perror ("Error opening provided file");
		return is_not_ok = EXIT_FAILURE;
	}
	else {
		while ((c = fgetc(fp)) != EOF)
		{
			// Is identifier? [a-zA-Z][a-zA-Z0-9]*
			if (isalpha(c))
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
			else if(isdigit(c) || c == '.') {
				std::string NumStr;
				do
				{
					NumStr += c;
					c = getchar();
				} while (isdigit(c) || c == '.');
				return tok.TOKEN_NUM;
			}
			else { 
				return tok.TOKEN_ID;
			}
			if (ferror(fp))
			puts("I/O error when reading");
			else if (feof(fp)) {
				puts("End of file is reached successfully");
				is_not_ok = EXIT_SUCCESS;
			}
		}
	}
}