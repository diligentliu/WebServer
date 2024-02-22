#ifndef WEBSERVER_HTTPRESPONSE_H
#define WEBSERVER_HTTPRESPONSE_H

#include <unordered_map>
#include <fcntl.h>          // open
#include <unistd.h>         // close
#include <sys/stat.h>       // stat
#include <sys/mman.h>       // mmap, munmap

#include "buffer.h"
#include "log.h"

using std::unordered_map;
using std::string;
using std::to_string;

class HttpResponse {
private:
	static const std::unordered_map<std::string, std::string> SUFFIX_TYPE;
	static const std::unordered_map<int, std::string> CODE_STATUS;
	static const std::unordered_map<int, std::string> CODE_PATH;

	int code_;
	bool is_keep_alive_;

	std::string path_;
	std::string src_dir_;

	char *mm_file_;
	struct stat mm_file_stat_;

private:
	void ErrorHtml_();

	void AddStateLine_(Buffer &buff);
	void AddHeader_(Buffer &buff);
	void AddContent_(Buffer &buff);

	std::string GetFileType_();

public:
	HttpResponse();
	~HttpResponse();

	void Init(const std::string &src_dir, std::string &path, bool is_keep_alive = false, int code = -1);
	void MakeResponse(Buffer &buff);
	char *File();
	size_t FileLen() const;
	void UnmapFile();
	void ErrorContent(Buffer &buff, std::string message);
	int Code() const { return code_; }

};

#endif //WEBSERVER_HTTPRESPONSE_H
