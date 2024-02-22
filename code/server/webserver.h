#ifndef WEBSERVER_WEBSERVER_H
#define WEBSERVER_WEBSERVER_H

#include <unordered_map>
#include <fcntl.h>       // fcntl()
#include <unistd.h>      // close()
#include <cassert>
#include <cerrno>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "epoller.h"
#include "log.h"
#include "heaptimer.h"
#include "sqlconnpool.h"
#include "threadpool.h"
#include "sqlconnRAII.h"
#include "httpconn.h"

class WebServer {
private:
	static const int MAX_FD = 65536;

	int port_;
	bool open_linger_;
	int timeout_ms_;
	bool is_close_;
	int listen_fd_;
	char *src_dir_;

	uint32_t listen_event_;
	uint32_t conn_event_;

	std::unique_ptr<HeapTimer> timer_;
	std::unique_ptr<ThreadPool> threadpool_;
	std::unique_ptr<Epoller> epoller_;
	std::unordered_map<int, HttpConn> users_;

private:
	bool InitSocket_();
	void InitEventMode_(int trig_mode);
	void AddClient_(int fd, sockaddr_in addr);

	void DealListen_();
	void DealWrite_(HttpConn *client);
	void DealRead_(HttpConn *client);

	void SendError_(int fd, const char *info);
	void ExtentTime_(HttpConn *client);
	void CloseConn_(HttpConn *client);

	void OnRead_(HttpConn *client);
	void OnWrite_(HttpConn *client);
	void OnProcess(HttpConn *client);

	static int SetFdNonblock(int fd);

public:
	WebServer(
			int port, int trig_mode, int timeout_ms, bool opt_linger,
			int sql_port, const char *sql_user, const char *sql_pwd,
			const char *db_name, int connPool_num, int thread_num,
			bool open_log, int log_level, int log_queue_size);

	~WebServer();
	void Start();

};

#endif //WEBSERVER_WEBSERVER_H
