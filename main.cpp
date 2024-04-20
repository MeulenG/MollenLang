#include "Lexer/Scanner.h"
#include "Lexer/Lexer.h"
#include "Lexer/Helper.h"


int main(int argc, char* argv[]) {
    // Check if a filename was passed
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " SOURCE_PROGRAM\n";
        return 1;
    }

    Scanner scan;
    int source_fd = open(argv[1], O_RDWR, 0);
	if (source_fd < 0)
	{
		perror("open source");
		return EXIT_FAILURE;
	}
    off_t fileSize = (scan.getFileSize(source_fd));
    std::vector<char> fileData = scan.readAndRemoveWS(source_fd, fileSize);
    scan.copyFile("testfile.txt", fileData);

    close(source_fd);

    std::ifstream file("build/testfile.txt");    
    Lexer lex;
    auto token = lex.isTokenIdentifier(file);
    if (token) {
        std::cout << "Token: " << token->text << ", Type: " << static_cast<int>(token->type) << std::endl;
    } else {
        std::cout << "No valid identifier found." << std::endl;
    }
    // for (int token = lex.Tokenize(argv[1]); token != tok.TOKEN_EOF; token++)
    // {
    //     printf("%d \n", token);
    // }
    
    return 0;
}