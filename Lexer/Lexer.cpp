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

int Lexer::scanFile(const char *fileName1, const char *fileName2, const char *fileName3) {
	// local variables
	int line = 1;
	char c;
	int is_not_ok = EXIT_FAILURE;

	// Copy the file for error handling
	int source_fd = open(fileName1, O_RDONLY, 0);
	if (source_fd < 0)
	{
		perror("open source");
		return is_not_ok = EXIT_FAILURE;
	}
	int temp_fd = open(fileName2, O_RDWR | O_CREAT | 	O_TRUNC, 0666);
	if (temp_fd < 0)
	{
		perror("open target");
		return is_not_ok = EXIT_FAILURE;
	}

	while (read(source_fd, &c, 1) > 0) {
        if (c != ' ' && c != '\t' && c != '\n') {
            if (write(temp_fd, &c, 1) == -1) {
                perror("Write to copied source program failed!");
                close(source_fd);
                close(temp_fd);
                return is_not_ok = EXIT_FAILURE;
            }
        }
		if (c == '\n')
		{
			line++;
		}
    }
	close(source_fd);
	close(temp_fd);

    source_fd = open(fileName2, O_RDONLY);  // Reopen file
    int target_fd = open(fileName3, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (target_fd == -1 || source_fd == -1) {
        perror("Failed to open file for sendfile");
        return EXIT_FAILURE;
    }
	struct stat stat;
	int r = fstat(source_fd, &stat);
	if (r < 0)
	{
		perror("fstat");
		return is_not_ok = EXIT_FAILURE;
	}
	off_t offset = 0;
	ssize_t bytes_sent = 0;
	ssize_t total_bytes_sent = 0;
	while (offset < stat.st_size) {
		bytes_sent = sendfile(target_fd, source_fd, &offset, stat.st_size - offset);
		total_bytes_sent += bytes_sent;
		if (bytes_sent < 0) {
			perror("\nsendfile\n");
			return EXIT_FAILURE;
		}
	}
	if (total_bytes_sent != stat.st_size) {
		fprintf(stderr, "\nsendfile: copied file truncated to %zd bytes\n", bytes_sent);
		return EXIT_FAILURE;
	} else {
		printf("\nsendfile: %zd bytes copied\n", total_bytes_sent);
	}

	close(source_fd); // source file
	close(target_fd); // target file

	unlink(fileName2);  // Clean up the temporary file

    return is_not_ok;
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
			else if(isdigit(c)) {

			}
			else { 

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