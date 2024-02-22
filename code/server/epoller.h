#ifndef WEBSERVER_EPOLLER_H
#define WEBSERVER_EPOLLER_H


#include <sys/epoll.h>  //epoll_ctl()
#include <fcntl.h>      // fcntl()
#include <unistd.h>     // close()
#include <cassert>
#include <cerrno>
#include <vector>

class Epoller {
private:
	int epoll_fd;

	std::vector<struct epoll_event> events_;

public:
	explicit Epoller(int max_event = 1024);
	~Epoller();

	bool AddFd(int fd, uint32_t events);
	bool ModFd(int fd, uint32_t events);
	bool DelFd(int fd);

	int Wait(int timeout_ms = -1);

	int GetEventFd(size_t i) const;
	uint32_t GetEvents(size_t i) const;
};


#endif //WEBSERVER_EPOLLER_H
