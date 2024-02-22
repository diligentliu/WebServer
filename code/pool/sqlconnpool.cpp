#include "sqlconnpool.h"

SqlConnPool::SqlConnPool() {
	use_count_ = 0;
	free_count_ = 0;
}

SqlConnPool::~SqlConnPool() {

}

SqlConnPool *SqlConnPool::Instance() {
	static SqlConnPool connPool;
	return &connPool;
}

void SqlConnPool::Init(const char *host, int port,
                       const char *user, const char *pwd,
                       const char *dbName, int connSize) {
	assert(connSize > 0);
	for (int i = 0; i < connSize; ++i) {
		MYSQL *sql = nullptr;
		sql = mysql_init(sql);
		if (!sql) {
			LOG_ERROR("Mysql init error!");
			assert(sql);
		}
		sql = mysql_real_connect(sql, host,
		                         user, pwd,
		                         dbName, port,
		                         nullptr, 0);
		if (!sql) {
			LOG_ERROR("Mysql Connect error!");
		}
		conn_que_.push(sql);
	}
	MAX_CONN_ = connSize;
	sem_init(&semId_, 0, MAX_CONN_);
}

MYSQL *SqlConnPool::GetConn() {
	MYSQL *sql = nullptr;
	if (conn_que_.empty()) {
		LOG_WARN("SqlConnPool busy!");
		return nullptr;
	}
	sem_wait(&semId_);
	{
		std::lock_guard<std::mutex> locker(mtx_);
		sql = conn_que_.front();
		conn_que_.pop();
	}
	return sql;
}

void SqlConnPool::FreeConn(MYSQL *conn) {
	assert(conn);
	std::lock_guard<std::mutex> locker(mtx_);
	conn_que_.push(conn);
	sem_post(&semId_);
}

int SqlConnPool::GetFreeConnCount() {
	std::lock_guard<std::mutex> locker(mtx_);
	return conn_que_.size();
}

void SqlConnPool::ClosePool() {
	std::lock_guard<std::mutex> locker(mtx_);
	while (!conn_que_.empty()) {
		auto item = conn_que_.front();
		conn_que_.pop();
		mysql_close(item);
	}
	mysql_library_end();
}
