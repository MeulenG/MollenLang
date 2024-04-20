#include "Scanner.h"

// Seek to the end of the file to find the size
int Scanner::getFileSize(int fd_in) {
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
int Scanner::openFileDescriptor(const char *file, const char *file2) {
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


std::vector<char> Scanner::readAndRemoveWS(int fd_in, off_t fileSize)
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
		
		if(c == '\n') {
			lines++;
		}
    }

	return filtered;
}

std::vector<char> Scanner::cleanComments(int fd_in, off_t fileSize)
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
		
		if(c == '\n') {
			lines++;
		}
    }

	return filtered;
}

void Scanner::copyFile(const char* file_out, const std::vector<char>& data) {
	int is_ok = EXIT_FAILURE;

	int target_fd = open(file_out, O_WRONLY | O_CREAT | O_TRUNC, 0666);
	if (target_fd < 0)
	{
		perror("open target");
	}

	size_t bytesWritten = write(target_fd, data.data(), data.size());
    if (bytesWritten == -1) {
        close(target_fd);
        perror("Error writing to file");
    }
	
	close(target_fd);
}
