#ifndef WEBSERVER_HTTPCONN_H
#define WEBSERVER_HTTPCONN_H

#include <sys/types.h>
#include <sys/uio.h>    // readv/writev
#include <arpa/inet.h>  // sockaddr_in
#include <cstdlib>      // atoi()
#include <cerrno>

#include "../log/log.h"
#include "../pool/sqlconnRAII.h"
#include "../buffer/buffer.h"
#include "httprequest.h"
#include "httpresponse.h"

class HttpConn {
private:
	int fd_;
	struct sockaddr_in addr_;

	bool is_close_;

	int iov_cnt_;
	struct iovec iov_[2];

	Buffer read_buff_; // 读缓冲区
	Buffer write_buff_; // 写缓冲区

	HttpRequest request_;
	HttpResponse response_;

public:
	static bool is_ET;
	static const char *src_dir;
	static std::atomic<int> user_count;

public:
	HttpConn();
	~HttpConn();

	void init(int sock_fd, const sockaddr_in &addr);

	void Close();
	int GetFd() const;
	sockaddr_in GetAddr() const;
	const char *GetIP() const;
	int GetPort() const;
	ssize_t read(int *save_errno);
	ssize_t write(int *save_errno);
	bool process();

	int ToWriteBytes() {
		return iov_[0].iov_len + iov_[1].iov_len;
	}

	bool IsKeepAlive() const {
		return request_.IsKeepAlive();
	}
};

#endif //WEBSERVER_HTTPCONN_H
