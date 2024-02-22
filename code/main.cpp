#include <unistd.h>
#include "webserver.h"

int main() {
	/* 守护进程 后台运行 */
	// daemon(1, 0);

	WebServer server(
			/* 端口 ET模式 timeoutms 优雅退出  */
			1316, 3, 60000, false,
			/* Mysql配置 */
			3306, "root", "root", "webserver",
			/* 连接池数量 线程池数量 日志开关 日志等级 日志异步队列容量 */
			12, 6, true, 1, 1024);
	server.Start();
}