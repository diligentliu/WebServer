#include "buffer.h"

/**
 * Private Function
 * BeginPtr_  : 缓冲区头部
 * MakeSpace_ : 分配空间
 */
char *Buffer::BeginPtr_() {
	return &*buffer_.begin();
}
const char *Buffer::BeginPtr_() const {
	return &*buffer_.begin();
}
// 这里有一个缺陷, 每次分配空间时, 要把未读的数据搬到缓冲区起始,
// 代替方案可以使用 ring_buffer 实现
void Buffer::MakeSpace_(size_t len) {
	if (WritableBytes() + PrependableBytes() < len) {
		buffer_.resize(writePos_ + len + 1);
	} else {
		size_t readable = ReadableBytes();
		std::copy(BeginPtr_() + readPos_, BeginPtr_() + writePos_, BeginPtr_());
		readPos_ = 0;
		writePos_ = readPos_ + readable;
		assert(readable == ReadableBytes());
	}
}
/**
 * Public Function
 */
Buffer::Buffer(int initBuffSize)
		: buffer_(initBuffSize), writePos_(0), readPos_(0) {
}
// 查看可写字节
size_t Buffer::WritableBytes() const {
	return buffer_.size() - writePos_;
}
// 查看可读字节
size_t Buffer::ReadableBytes() const {
	return writePos_ - readPos_;
}
// 返回读指针之前的字节, 这部分被划分为头部区域, 同样是可写的
size_t Buffer::PrependableBytes() const {
	return readPos_;
}
// 返回读指针在内存中的位置
const char *Buffer::Peek() const {
	return BeginPtr_() + readPos_;
}
// 标记这些长度的数据为已读
void Buffer::Retrieve(size_t len) {
	assert(len <= ReadableBytes());
	readPos_ += len;
}
// 标记直到 end 的数据为已读
void Buffer::RetrieveUntil(const char *end) {
	assert(Peek() <= end);
	Retrieve(end - Peek());
}
// 标记所有数据为已读
void Buffer::RetrieveAll() {
	bzero(&buffer_[0], buffer_.size());
	readPos_ = 0;
	writePos_ = 0;
}
// 标记所有数据为已读, 并返回未读取的数据内容
std::string Buffer::RetrieveAllToStr() {
	std::string str(Peek(), ReadableBytes());
	RetrieveAll();
	return str;
}
// 返回写指针在内存中的位置
const char *Buffer::BeginWriteConst() const {
	return BeginPtr_() + writePos_;
}
char *Buffer::BeginWrite() {
	return BeginPtr_() + writePos_;
}
// 将 len 长度的数据标记为已写
void Buffer::HasWritten(size_t len) {
	writePos_ += len;
}
// 写数据
void Buffer::Append(const std::string &str) {
	Append(str.data(), str.length());
}
void Buffer::Append(const void *data, size_t len) {
	assert(data);
	Append(static_cast<const char *>(data), len);
}
void Buffer::Append(const char *str, size_t len) {
	assert(str);
	EnsureWriteable(len);
	std::copy(str, str + len, BeginWrite());
	HasWritten(len);
}

void Buffer::Append(const Buffer &buff) {
	Append(buff.Peek(), buff.ReadableBytes());
}

void Buffer::EnsureWriteable(size_t len) {
	if (WritableBytes() < len) {
		MakeSpace_(len);
	}
	assert(WritableBytes() >= len);
}

ssize_t Buffer::ReadFd(int fd, int *saveErrno) {
	char buff[65535];
	struct iovec iov[2];
	const size_t writable = WritableBytes();
	/*
	 * 分散读， 保证数据全部读完
	 * buff 的目的是用于存储读取的额外数据,
	 * 以防止在缓冲区的当前写指针位置不足以容纳所有数据时,
	 * 进行额外的存储。
	 */
	iov[0].iov_base = BeginPtr_() + writePos_;
	iov[0].iov_len = writable;
	iov[1].iov_base = buff;
	iov[1].iov_len = sizeof(buff);

	const ssize_t len = readv(fd, iov, 2);
	if (len < 0) {
		*saveErrno = errno;
	} else if (static_cast<size_t>(len) <= writable) {
		writePos_ += len;
	} else {
		writePos_ = buffer_.size();
		Append(buff, len - writable);
	}
	return len;
}

ssize_t Buffer::WriteFd(int fd, int *saveErrno) {
	size_t readSize = ReadableBytes();
	ssize_t len = write(fd, Peek(), readSize);
	if (len < 0) {
		*saveErrno = errno;
		return len;
	}
	readPos_ += len;
	return len;
}
