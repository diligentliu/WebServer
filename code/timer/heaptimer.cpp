#include "heaptimer.h"

void HeapTimer::del_(size_t i) {
	// 删除指定位置的节点
	assert(!heap_.empty() && i >= 0 && i < heap_.size());
	size_t n = heap_.size() - 1;
	assert(i <= n);
	if (i < n) {
		swap_node_(i, n);
		if (!siftdown_(i, n)) {
			siftup_(i);
		}
	}
	ref_.erase(heap_.back().id);
	heap_.pop_back();
}

void HeapTimer::siftup_(size_t i) {
	assert(i >= 0 && i < heap_.size());
	size_t j = (i - 1) / 2;
	while (j >= 0) {
		if (heap_[j] < heap_[i]) {
			break;
		}
		swap_node_(i, j);
		i = j;
		j = (i - 1) / 2;
	}
}

bool HeapTimer::siftdown_(size_t index, size_t n) {
	assert(index >= 0 && index < heap_.size() && n >= 0 && n <= heap_.size());
	size_t i = index, j = i * 2 + 1;
	while (j < n) {
		if (j + 1 < n && heap_[j + 1] < heap_[j]) {
			++j;
		}
		if (heap_[i] < heap_[j]) {
			break;
		}
		swap_node_(i, j);
		i = j;
		j = i * 2 + 1;
	}
	return i > index;
}

void HeapTimer::swap_node_(size_t i, size_t j) {
	assert(i >= 0 && i < heap_.size() && j >= 0 && j < heap_.size());
	std::swap(heap_[i], heap_[j]);
	ref_[heap_[i].id] = i;
	ref_[heap_[j].id] = j;
}

void HeapTimer::adjust(int id, int newExpires) {
	// 调整指定 id 节点的 time_out
	assert(!heap_.empty() && ref_.count(id) > 0);
	heap_[ref_[id]].expires = Clock::now() + ms(newExpires);
	if (!siftdown_(ref_[id], heap_.size())) {
		siftup_(ref_[id]);
	}
}

void HeapTimer::add(int id, int time_out, const TimeoutCallBack &cb) {
	assert(id >= 0);
	size_t i;
	if (ref_.count(id) == 0) {
		// 新节点, 堆尾插入, 调整堆
		i = heap_.size();
		ref_[id] = i;
		heap_.emplace_back(id, Clock::now() + ms(time_out), cb);
		siftup_(i);
	} else {
		// 已有节点, 调整堆
		i = ref_[id];
		heap_[i].expires = Clock::now() + ms(time_out);
		heap_[i].cb = cb;
		if (!siftdown_(i, heap_.size())) {
			siftup_(i);
		}
	}
}

void HeapTimer::doWork(int id) {
	// 删除指定 id, 触发回调函数
	if (heap_.empty() || ref_.count(id) == 0) {
		return;
	}
	size_t i = ref_[id];
	TimerNode &node = heap_[i];
	node.cb;
	del_(i);
}

void HeapTimer::clear() {
	ref_.clear();
	heap_.clear();
}

void HeapTimer::tick() {
	// 清除超时节点
	if (heap_.empty()) {
		return;
	}
	while (!heap_.empty()) {
		TimerNode &node = heap_.front();
		if (std::chrono::duration_cast<ms>(node.expires - Clock::now()).count() > 0) {
			break;
		}
		node.cb();
		pop();
	}
}

void HeapTimer::pop() {
	assert(!heap_.empty());
	del_(0);
}

int HeapTimer::GetNextTick() {
	tick();
	size_t res = -1;
	if (!heap_.empty()) {
		res = std::chrono::duration_cast<ms>(heap_.front().expires - Clock::now()).count();
		if (res < 0) {
			res = 0;
		}
	}
	return res;
}
