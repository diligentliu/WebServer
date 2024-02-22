#ifndef WEBSERVER_BLOCKDEQUE_H
#define WEBSERVER_BLOCKDEQUE_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include <sys/time.h>
#include <cassert>

template<typename T>
class BlockDeque {
private:
	std::deque<T> deq_;
	size_t capacity_;
	std::mutex mtx_;
	bool is_close_;
	std::condition_variable condConsumer_;
	std::condition_variable condProducer_;

public:
	explicit BlockDeque(size_t max_capacity = 1000);
	~BlockDeque();

	void flush();

	void clear();
	bool empty();
	bool full();
	void close();

	size_t size();
	size_t capacity();

	T front();
	T back();

	void push_back(const T &item);
	void push_front(const T &item);
	bool pop(T &item);
	bool pop(T &item, int timeout);
};

template<typename T>
BlockDeque<T>::BlockDeque(size_t max_capacity)
		: capacity_(max_capacity) {
	assert(max_capacity > 0);
	is_close_ = false;
}

template<typename T>
BlockDeque<T>::~BlockDeque() {
	close();
}

template<typename T>
void BlockDeque<T>::flush() {
	condConsumer_.notify_one();
}

template<typename T>
void BlockDeque<T>::clear() {
	std::lock_guard<std::mutex> locker(mtx_);
	deq_.clear();
}

template<typename T>
bool BlockDeque<T>::empty() {
	std::lock_guard<std::mutex> locker(mtx_);
	return deq_.empty();
}

template<typename T>
bool BlockDeque<T>::full() {
	std::lock_guard<std::mutex> locker(mtx_);
	return deq_.size() >= capacity_;
}

template<typename T>
void BlockDeque<T>::close() {
	{
		std::lock_guard<std::mutex> locker(mtx_);
		deq_.clear();
		is_close_ = true;
	}
	condProducer_.notify_all();
	condConsumer_.notify_all();
}

template<typename T>
size_t BlockDeque<T>::size() {
	std::lock_guard<std::mutex> locker(mtx_);
	return deq_.size();
}

template<typename T>
size_t BlockDeque<T>::capacity() {
	std::lock_guard<std::mutex> locker(mtx_);
	return capacity_;
}

template<typename T>
T BlockDeque<T>::front() {
	std::lock_guard<std::mutex> locker(mtx_);
	return deq_.front();
}

template<typename T>
T BlockDeque<T>::back() {
	std::lock_guard<std::mutex> locker(mtx_);
	return deq_.back();
}

template<typename T>
void BlockDeque<T>::push_back(const T &item) {
	std::unique_lock<std::mutex> locker(mtx_);
	while (deq_.size() >= capacity_) {
		condProducer_.wait(locker);
	}
	deq_.push_back(item);
	condConsumer_.notify_one();
}

template<typename T>
void BlockDeque<T>::push_front(const T &item) {
	std::unique_lock<std::mutex> locker(mtx_);
	while (deq_.size() >= capacity_) {
		condProducer_.wait(locker);
	}
	deq_.push_front(item);
	condConsumer_.notify_one();
}

template<typename T>
bool BlockDeque<T>::pop(T &item) {
	std::unique_lock<std::mutex> locker(mtx_);
	while (deq_.empty()) {
		condConsumer_.wait(locker);
		if (is_close_) {
			return false;
		}
	}
	item = deq_.front();
	deq_.pop_front();
	condProducer_.notify_one();
	return true;
}

template<typename T>
bool BlockDeque<T>::pop(T &item, int timeout) {
	std::unique_lock<std::mutex> locker(mtx_);
	while (deq_.empty()) {
		if (condConsumer_.wait_for(locker, std::chrono::seconds(timeout))
		    == std::cv_status::timeout) {
			return false;
		}
	}
	item = deq_.front();
	deq_.pop_front();
	condProducer_.notify_one();
	return true;
}

#endif //WEBSERVER_BLOCKDEQUE_H
