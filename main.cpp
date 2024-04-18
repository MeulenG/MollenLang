#include <iostream>
#include <fstream>
#include <string>
#include "Lexer/Lexer.h"
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <vector>


int main(int argc, char* argv[]) {
    // Check if a filename was passed
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " SOURCE_PROGRAM\n";
        return 1;
    }

    Lexer lex;
    Token tok;
    int source_fd = open(argv[1], O_RDWR, O_APPEND, 0);
	if (source_fd < 0)
	{
		perror("open source");
		return EXIT_FAILURE;
	}
    off_t fileSize = ("%d", lex.getFileSize(source_fd));
    std::vector<char> fileData = lex.readAndRemoveWS(source_fd, fileSize);
    lex.copyFile(argv[1], "testfile.txt", fileData);
    close(source_fd);
    
    // lex.readFileDescriptor(argv[1], "testfile.txt");
    // for (int token = lex.Tokenize(argv[1]); token != tok.TOKEN_EOF; token++)
    // {
    //     printf("%d \n", token);
    // }
    
    return 0;
}