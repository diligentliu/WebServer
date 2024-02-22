#ifndef WEBSERVER_HTTPREQUEST_H
#define WEBSERVER_HTTPREQUEST_H

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <regex>
#include <cerrno>
#include <mysql/mysql.h>  //mysql

#include "buffer.h"
#include "log.h"
#include "sqlconnpool.h"
#include "sqlconnRAII.h"

using std::unordered_set;
using std::unordered_map;
using std::string;
using std::search;
using std::regex;
using std::smatch;

class HttpRequest {
public:
	enum PARSE_STATE {
		REQUEST_LINE,
		HEADERS,
		BODY,
		FINISH,
	};

	enum HTTP_CODE {
		NO_REQUEST = 0,
		GET_REQUEST,
		BAD_REQUEST,
		NO_RESOURSE,
		FORBIDDENT_REQUEST,
		FILE_REQUEST,
		INTERNAL_ERROR,
		CLOSED_CONNECTION,
	};

private:
	static const unordered_set<string> DEFAULT_HTML;
	static const unordered_map<string, int> DEFAULT_HTML_TAG;

	PARSE_STATE state_;
	string method_, path_, version_, body_;
	unordered_map<string, string> header_;
	unordered_map<string, string> post_;

	bool ParseRequestLine_(const string &line);
	void ParseHeader_(const string &line);
	void ParseBody_(const string &line);

	void ParsePath_();
	void ParsePost_();
	void ParseFromUrlencoded_();

	static int ConverHex(char ch);
	static bool UserVerify(const string &name, const string &pwd, bool is_login);

public:
	HttpRequest() { Init(); }
	~HttpRequest() = default;

	void Init();
	bool parse(Buffer &buff);

	std::string path() const;
	std::string &path();
	std::string method() const;
	std::string version() const;
	std::string GetPost(const string &key) const;
	std::string GetPost(const char *key) const;

	bool IsKeepAlive() const;
	/*
		todo
		void HttpConn::ParseFormData() {}
		void HttpConn::ParseJson() {}
	*/
};


#endif //WEBSERVER_HTTPREQUEST_H
