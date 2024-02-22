#include "epoller.h"

Epoller::Epoller(int max_event)
		: epoll_fd(epoll_create(1)), events_(max_event) {
	assert(max_event > 0);
	assert(epoll_fd >= 0 && events_.size() > 0);
}

Epoller::~Epoller() {
	close(epoll_fd);
}

bool Epoller::AddFd(int fd, uint32_t events) {
	if (fd < 0) {
		return false;
	}
	epoll_event e = {0};
	e.data.fd = fd;
	e.events = events;
	return epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &e) == 0;
}

bool Epoller::ModFd(int fd, uint32_t events) {
	if (fd < 0) {
		return false;
	}
	epoll_event e = {0};
	e.data.fd = fd;
	e.events = events;
	return epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &e) == 0;
}

bool Epoller::DelFd(int fd) {
	if (fd < 0) {
		return false;
	}
	epoll_event e = {0};
	e.data.fd = fd;
	return epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, &e) == 0;
}

int Epoller::Wait(int timeout_ms) {
	return epoll_wait(epoll_fd,
	                  &events_[0],
	                  static_cast<int>(events_.size()),
	                  timeout_ms);
}

int Epoller::GetEventFd(size_t i) const {
	assert(i < events_.size() && i >= 0);
	return events_[i].data.fd;
}

uint32_t Epoller::GetEvents(size_t i) const {
	assert(i < events_.size() && i >= 0);
	return events_[i].events;
}

