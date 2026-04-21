#pragma once
#include "Singleton.h"
#include <queue>
#include <thread>
#include "CSession.h"
#include <queue>
#include <map>
#include <functional>
#include "const.h"
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
#include <unordered_map>
#include "LogicWorker.h"

class FileInfo {
public:
	FileInfo(int seq = 0, std::string name = "", int total_size = 0,
		int trans_size = 0, std::string file_path_str = "")
		:_seq(seq), _name(name), _total_size(total_size),
		_trans_size(trans_size), _file_path_str(file_path_str) {}
	int _seq;
	std::string _name;
	int _total_size;
	int _trans_size;
	std::string _file_path_str;
};

typedef  function<void(shared_ptr<CSession>, const short &msg_id, const string &msg_data)> FunCallBack;
class LogicSystem:public Singleton<LogicSystem>
{
	friend class Singleton<LogicSystem>;
public:
	~LogicSystem();
	void PostMsgToQue(shared_ptr < LogicNode> msg, int index);
	void AddMD5File(std::string md5, std::shared_ptr<FileInfo> fileinfo);
	std::shared_ptr<FileInfo> GetFileInfo(std::string md5);
private:
	LogicSystem();
	std::vector<std::shared_ptr<LogicWorker> > _workers;
	std::mutex _file_mtx;
	std::unordered_map<std::string, std::shared_ptr<FileInfo>> _map_md5_files;
};

