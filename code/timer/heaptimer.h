#ifndef WEBSERVER_HEAPTIMER_H
#define WEBSERVER_HEAPTIMER_H

#include <queue>
#include <unordered_map>
#include <ctime>
#include <algorithm>
#include <arpa/inet.h>
#include <functional>
#include <cassert>
#include <chrono>
#include "log.h"

typedef std::function<void()> TimeoutCallBack;
typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::milliseconds ms;
typedef Clock::time_point TimeStamp;

struct TimerNode {
	int id;
	TimeStamp expires;
	TimeoutCallBack cb;
	bool operator<(const TimerNode &t) const {
		return expires < t.expires;
	}
};

/**
 * 成员变量：
 *  heap_: 用于存储定时任务的小顶堆，按照任务的超时时间排序。
 *  ref_: 用于保存任务在堆中的索引，以及任务 ID 到索引的映射。
 * 成员函数：
 *  del_ 函数：
 *      删除指定位置的任务，并保持小顶堆性质。
 *  siftup_ 函数：
 *      用于执行小顶堆的向上调整操作，保持小顶堆性质。
 *  siftdown_ 函数：
 *      用于执行小顶堆的向下调整操作，保持小顶堆性质。
 *  swap_node_ 函数：
 *      交换堆中两个节点的位置，并更新节点在 ref_ 中的索引。
 *  adjust 函数：
 *      调整指定 ID 的任务的超时时间。
 *  add 函数：
 *      向定时器中添加任务，如果任务已存在则更新超时时间。
 *  doWork 函数：
 *      执行指定 ID 的任务，即调用任务的回调函数，然后从定时器中删除该任务。
 *  clear 函数：
 *      清空定时器中的所有任务。
 *  tick 函数：
 *      处理定时器的主要逻辑，即清除已超时的任务，并执行它们的回调函数。
 *  pop 函数：
 *      弹出小顶堆的堆顶元素，即最小的超时任务，并从定时器中删除该任务。
 *  GetNextTick 函数：
 *      获取距离下一个任务超时的时间，用于调度等待操作。
 */
class HeapTimer {
private:
	std::vector<TimerNode> heap_;
	std::unordered_map<int, size_t> ref_;

private:
	void del_(size_t i);
	/**
	 * 用于执行小顶堆的向上调整操作, 保持小顶堆性质,
	 * 当在数组末端添加新节点后, 将节点依次和父节点比较
	 * 如果小于父节点, 交换
	 */
	void siftup_(size_t i);
	bool siftdown_(size_t index, size_t n);
	void swap_node_(size_t i, size_t j);

public:
	HeapTimer() { heap_.reserve(64); }
	~HeapTimer() { clear(); }

	void adjust(int id, int newExpires);
	void add(int id, int time_out, const TimeoutCallBack &cb);
	void doWork(int id);
	void clear();
	void tick();
	void pop();
	int GetNextTick();

};

#endif //WEBSERVER_HEAPTIMER_H
