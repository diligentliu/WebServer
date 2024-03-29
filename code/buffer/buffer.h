#ifndef WEBSERVER_BUFFER_H
#define WEBSERVER_BUFFER_H

#include <cstring>      // perror
#include <iostream>
#include <unistd.h>     // write
#include <sys/uio.h>    // readv
#include <vector>
#include <atomic>
#include <cassert>

class Buffer {
private:
	std::vector<char> buffer_;
	std::atomic<std::size_t> readPos_;
	std::atomic<std::size_t> writePos_;

private:
	char *BeginPtr_();
	const char *BeginPtr_() const;
	void MakeSpace_(size_t len);

public:
	Buffer(int initBuffSize = 1024);
	~Buffer() = default;

	size_t WritableBytes() const;
	size_t ReadableBytes() const;
	size_t PrependableBytes() const;

	const char *Peek() const;
	void EnsureWriteable(size_t len);
	void HasWritten(size_t len);

	void Retrieve(size_t len);
	void RetrieveUntil(const char *end);

	void RetrieveAll();
	std::string RetrieveAllToStr();

	const char *BeginWriteConst() const;
	char *BeginWrite();

	void Append(const std::string &str);
	void Append(const char *str, size_t len);
	void Append(const void *data, size_t len);
	void Append(const Buffer &buff);

	ssize_t ReadFd(int fd, int *Errno);
	ssize_t WriteFd(int fd, int *Errno);
};


#endif //WEBSERVER_BUFFER_H
