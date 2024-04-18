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
#include <vector>

// Seek to the end of the file to find the size
int Lexer::getFileSize(int fd_in) {
    off_t fileSize = lseek(fd_in, 0, SEEK_END);
    if (fileSize == -1) {
        return -1;
    }

    if (lseek(fd_in, 0, SEEK_SET) == -1) {
        return -1;
    }

    return fileSize;
}


// Legit useless function lmfao
int Lexer::openFileDescriptor(const char *file, const char *file2) {
	int is_ok = EXIT_FAILURE;

	
	int source_fd = open(file, O_RDWR, O_APPEND, 0);
	if (source_fd < 0)
	{
		perror("open source");
		return is_ok = EXIT_FAILURE;
	}

	int target_fd = open(file2, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (target_fd < 0)
	{
		perror("open target");
		return is_ok = EXIT_FAILURE;
	}
	
	close(source_fd);
	close(target_fd);
    return is_ok = EXIT_FAILURE;
}


std::vector<char> Lexer::readAndRemoveWS(int fd_in, off_t fileSize)
{
    off_t i = 0, j = 0;
    std::vector<char> original(fileSize);
	std::vector<char> filtered(fileSize, 0);

    if (read(fd_in, &original[0], fileSize) != fileSize) {
        perror("Couldn't read entire file");
    }
    
    while (i < fileSize) {
        char c = original[i++];
        if (!isspace(c)) {
            filtered[j++] = c;
        }
    }

	return filtered;
}

void Lexer::copyFile(const char* file, const char* file_out, const std::vector<char>& data) {
	int is_ok = EXIT_FAILURE;

	int source_fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0666);
	if (source_fd < 0)
	{
		perror("Error opening and copying file");
	}

	int target_fd = open(file_out, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (target_fd < 0)
	{
		perror("open target");
	}

	size_t bytesWritten = write(target_fd, data.data(), data.size());
    if (bytesWritten == -1) {
        close(source_fd);
        perror("Error writing to file");
    }

	close(source_fd);
}


bool Lexer::is_digit(char c) noexcept {
	switch (c) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			return true;
		default:
			return false;
	}
}

bool Lexer::is_identifier(char c) noexcept {
	switch (c) {
		case 'a':
		case 'b':
		case 'c':
		case 'd':
		case 'e':
		case 'f':
		case 'g':
		case 'h':
		case 'i':
		case 'j':
		case 'k':
		case 'l':
		case 'm':
		case 'n':
		case 'o':
		case 'p':
		case 'q':
		case 'r':
		case 's':
		case 't':
		case 'u':
		case 'v':
		case 'w':
		case 'x':
		case 'y':
		case 'z':
		case 'A':
		case 'B':
		case 'C':
		case 'D':
		case 'E':
		case 'F':
		case 'G':
		case 'H':
		case 'I':
		case 'J':
		case 'K':
		case 'L':
		case 'M':
		case 'N':
		case 'O':
		case 'P':
		case 'Q':
		case 'R':
		case 'S':
		case 'T':
		case 'U':
		case 'V':
		case 'W':
		case 'X':
		case 'Y':
		case 'Z':
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case '_':
			return true;
		default:
			return false;
	}
}


int Lexer::Tokenize(const char *fileName) {
	int line = 1;
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
			// Is identifier? [a-zA-Z][a-zA-Z0-9]*
			if (is_identifier(c))
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
			else if(is_digit(c) || c == '.') {
				std::string NumStr;
				do
				{
					NumStr += c;
					c = getchar();
				} while (is_digit(c) || c == '.');
				return tok.TOKEN_NUM;
			}
			else { 
				return tok.TOKEN_ID;
			}
			if (ferror(fp))
			puts("I/O error when reading");
			else if (feof(fp)) {
				puts("End of file is reached successfully");
				is_ok = EXIT_SUCCESS;
			}
		}
	}
}