#ifndef WEBSERVER_LOG_H
#define WEBSERVER_LOG_H

#include <mutex>
#include <string>
#include <thread>
#include <sys/time.h>
#include <cstring>
#include <cstdarg>              // vastart va_end
#include <cassert>
#include <sys/stat.h>           //mkdir
#include "blockdeque.h"
#include "buffer.h"

class Log {
private:
	static const int LOG_PATH_LEN = 256;
	static const int LOG_NAME_LEN = 256;
	static const int MAX_LINES = 50000;

	const char *path_;
	const char *suffix_;

	int MAX_LINES_;

	int line_count_;
	int to_day_;

	bool is_open_;

	Buffer buff_;
	int level_;
	bool is_async_;

	FILE *fp_;
	std::unique_ptr<BlockDeque<std::string>> deque_;
	std::unique_ptr<std::thread> write_thread_;
	std::mutex mtx_;

private:
	Log();
	virtual ~Log();

	void AppendLogLevelTitle_(int level);
	void AsyncWrite_();

public:
	void init(int level, const char *path = "./log",
	          const char *suffix = ".log",
	          int maxQueueCapacity = 1024);

	void write(int level, const char *format, ...);
	void flush();

	int GetLevel();
	void SetLevel(int level);
	bool IsOpen() { return is_open_; }

	static Log *Instance();
	static void FlushLogThread();
};

#define LOG_BASE(level, format, ...) \
    do {\
        Log* log = Log::Instance();\
        if (log->IsOpen() && log->GetLevel() <= level) {\
            log->write(level, format, ##__VA_ARGS__); \
            log->flush();\
        }\
    } while(0);

#define LOG_DEBUG(format, ...) do {LOG_BASE(0, format, ##__VA_ARGS__)} while(0);
#define LOG_INFO(format, ...) do {LOG_BASE(1, format, ##__VA_ARGS__)} while(0);
#define LOG_WARN(format, ...) do {LOG_BASE(2, format, ##__VA_ARGS__)} while(0);
#define LOG_ERROR(format, ...) do {LOG_BASE(3, format, ##__VA_ARGS__)} while(0);


#endif //WEBSERVER_LOG_H
