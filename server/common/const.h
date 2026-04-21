#pragma once
#include <boost/beast/http.hpp>
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <memory>
#include <iostream>
#include "Singleton.h"
#include <functional>
#include <map>
#include <unordered_map>
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <atomic>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <hiredis/hiredis.h>
#include <cassert>


namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

enum ErrorCodes {
	Success = 0,
	Error_Json = 1001,  //Json解析失败
	RPCFailed = 1002,  //RPC失败
	VarifyExpired = 1003, //验证过期
	VarifyCodeErr = 1004, //验证码错误
	UserExist = 1005,       //用户已存在
	PasswdErr = 1006,    //密码错误
	EmailNotMatch = 1007,  //邮箱不匹配
	PasswdUpFailed = 1008,  //密码更新失败
	PasswdInvalid = 1009,   //密码无效
	TokenInvalid = 1010,   //Token无效
	UidInvalid = 1011,  //uid无效
	CREATE_CHAT_FAILED = 1012,	//创建聊天失败
	LOAD_CHAT_FAILED = 1013	//加载聊天失败
};

class Defer {
public:
	Defer(std::function<void()> func) : func_(func) {}
	~Defer() { func_(); };
private:
	std::function<void()> func_;
};

#define CODEPREFIX "code_"

#define MAX_LENGTH 1024*2
//头部总长度
#define HEAD_TOTAL_LEN 4
//头部id长度
#define HEAD_ID_LEN 2
//头部数据长度
#define HEAD_DATA_LEN 2
#define MAX_RECVQUE 10000
#define MAX_SENDQUE 1000

enum MSG_IDS {
	MSG_CHAT_LOGIN = 1005, //用户登录
	MSG_CHAT_LOGIN_RSP = 1006, //用户登录回复
	ID_SEARCH_USER_REQ = 1007, //用户搜索请求
	ID_SEARCH_USER_RSP = 1008, //用户搜索回复
	ID_ADD_FRIEND_REQ = 1009, //添加好友请求
	ID_ADD_FRIEND_RSP = 1010, //添加好友回复
	ID_NOTIFY_ADD_FRIEND_REQ = 1011,  //通知用户添加好友请求
	ID_AUTH_FRIEND_REQ = 1013,  //认证好友请求
	ID_AUTH_FRIEND_RSP = 1014,  //认证好友回复
	ID_NOTIFY_AUTH_FRIEND_REQ = 1015, //通知用户认证好友请求
	ID_TEXT_CHAT_MSG_REQ = 1017, //文本聊天信息请求
	ID_TEXT_CHAT_MSG_RSP = 1018, //文本聊天信息回复
	ID_NOTIFY_TEXT_CHAT_MSG_REQ = 1019, //通知用户文本聊天信息
	ID_NOTIFY_OFF_LINE_REQ = 1021, //通知用户下线
	ID_HEART_BEAT_REQ = 1023,      //心跳请求
	ID_HEARTBEAT_RSP = 1024,       //心跳回复
	ID_LOAD_CHAT_THREAD_REQ = 1025,	//加载聊天线程请求
	ID_LOAD_CHAT_THREAD_RSP = 1026,	//聊天线程回复
	ID_CREATE_PRIVATE_CHAT_REQ = 1027,	//创建聊天表请求
	ID_CREATE_PRIVATE_CHAT_RSP = 1028,	//创建聊天表回复
	ID_LOAD_CHAT_MSG_REQ = 1029,
	ID_LOAD_CHAT_MSG_RSP = 1030,

	ID_IMG_CHAT_MSG_REQ = 1035,
	ID_IMG_CHAT_MSG_RSP = 1036,
	ID_NOTIFY_IMG_CHAT_MSG_REQ = 1039,
};

#define USERIPPREFIX  "uip_"
#define USERTOKENPREFIX  "utoken_"
#define IPCOUNTPREFIX  "ipcount_"
#define USER_BASE_INFO "ubaseinfo_"
#define LOGIN_COUNT  "logincount"
#define NAME_INFO  "nameinfo_"
#define LOCK_PREFIX "lock_"
#define USERSESSIONPREFIX "usession_"
#define LOCK_COUNT "lockcount"

//锁超时时间
#define LOCK_TIME_OUT 10
//锁获取超时时间
#define ACQUIRE_TIME_OUT 5
//心跳阈值
#define HEART_THRESHOLD 20

enum MsgStatus {
	UN_READ = 0,
	SEND_FAILED = 1,
	READED = 2,
	UN_UPLOAD = 3,
};