#include <iostream>
#include <fstream>
#include <string>
#include "Lexer/Lexer.h"
#include "Lexer/Scanner.h"
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

    Scanner scan;
    Lexer lex;
    Token tok;
    int source_fd = open(argv[1], O_RDWR, 0);
	if (source_fd < 0)
	{
		perror("open source");
		return EXIT_FAILURE;
	}
    off_t fileSize = ("%d", scan.getFileSize(source_fd));
    std::vector<char> fileData = scan.readAndRemoveWS(source_fd, fileSize);
    scan.copyFile("testfile.txt", fileData);

    
    // for (int token = lex.Tokenize(argv[1]); token != tok.TOKEN_EOF; token++)
    // {
    //     printf("%d \n", token);
    // }
    
    close(source_fd);
    return 0;
}