#ifndef WEBSERVER_SQLCONNPOOL_H
#define WEBSERVER_SQLCONNPOOL_H

#include <mysql/mysql.h>
#include <string>
#include <queue>
#include <mutex>
#include <semaphore.h>
#include <thread>
#include "log.h"

class SqlConnPool {
private:
	int MAX_CONN_;
	int use_count_;
	int free_count_;

	std::queue<MYSQL *> conn_que_;
	std::mutex mtx_;
	sem_t semId_;

private:
	SqlConnPool();
	~SqlConnPool();

public:
	static SqlConnPool *Instance();

	void Init(const char *host, int port,
	          const char *user, const char *pwd,
	          const char *dbName, int connSize);

	MYSQL *GetConn();
	void FreeConn(MYSQL *conn);
	int GetFreeConnCount();

	void ClosePool();

};

#endif //WEBSERVER_SQLCONNPOOL_H
