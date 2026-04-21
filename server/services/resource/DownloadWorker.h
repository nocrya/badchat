#pragma once
#include "const.h"
#include "CSession.h"
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>

struct DownloadTask {
    DownloadTask(std::shared_ptr<CSession> session, int uid, std::string path, std::string name,
        int seq, std::function<void(const Json::Value&)> callback) :_session(session), _uid(uid),
        _seq(seq), _file_path(path), _name(name), _callback(callback)
    {}
    ~DownloadTask() {}
    std::shared_ptr<CSession> _session;
    int _uid;
    int _seq;
    std::string _file_path;
    std::string _name;
    std::function<void(const Json::Value&)>  _callback;  //回调函数
};

class DownloadWorker
{
public:
	DownloadWorker();
	~DownloadWorker();
	void PostTask(std::shared_ptr<DownloadTask> task);
private:
	void task_callback(std::shared_ptr<DownloadTask>);
	std::thread _work_thread;
	std::queue<std::shared_ptr<DownloadTask>> _task_que;
	std::atomic<bool> _b_stop;
	std::mutex  _mtx;
	std::condition_variable _cv;
};

