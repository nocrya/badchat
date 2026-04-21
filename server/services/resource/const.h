#pragma once
#include <functional>
#include <queue>
#include <mutex>

enum ErrorCodes {
	Success = 0,
	Error_Json = 1001,  //Json解析错误
	RPCFailed = 1002,  //RPC请求错误
	VarifyExpired = 1003, //验证码过期
	VarifyCodeErr = 1004, //验证码错误
	UserExist = 1005,       //用户已经存在
	PasswdErr = 1006,    //密码错误
	EmailNotMatch = 1007,  //邮箱不匹配
	PasswdUpFailed = 1008,  //更新密码失败
	PasswdInvalid = 1009,   //密码更新失败
	TokenInvalid = 1010,   //Token失效
	UidInvalid = 1011,  //uid无效
	FileNotExists = 1012, //文件不存在
	FileSaveRedisFailed = 1013,
	FileWritePermissionFailed = 1014,
	FileReadPermissionFailed = 1015,
	FileOffsetInvalid = 1016,
	FileSeqInvalid = 1017,
	RedisReadErr = 1018,
	FileReadFailed = 1019,
};


// Defer类
class Defer {
public:
	// 接受一个lambda表达式或者函数指针
	Defer(std::function<void()> func) : func_(func) {}

	// 析构函数中执行传入的函数
	~Defer() {
		func_();
	}

private:
	std::function<void()> func_;
};

#define MAX_LENGTH  1024*64
//头部总长度
#define HEAD_TOTAL_LEN 6
//头部id长度
#define HEAD_ID_LEN 2
//头部数据长度
#define HEAD_DATA_LEN 4
#define MAX_RECVQUE  2000000
#define MAX_SENDQUE 2000000

#define MAX_FILE_LEN 2048

//4个逻辑工作者
#define LOGIC_WORKER_COUNT 4
//4个文件工作者
#define FILE_WORKER_COUNT 4

#define DOWN_LOAD_WORKER_COUNT 4


enum MSG_IDS {
	ID_TEST_MSG_REQ = 1001,       //测试消息
	ID_TEST_MSG_RSP = 1002,       //测试消息回包
	ID_UPLOAD_FILE_REQ = 1003,    //发送文件请求
	ID_UPLOAD_FILE_RSP = 1004,    //发送文件回复
	ID_SYNC_FILE_REQ = 1005,      //同步文件信息请求
	ID_SYNC_FILE_RSP = 1006,      //同步文件回复回复
	ID_UPLOAD_HEAD_ICON_REQ = 1031,
	ID_UPLOAD_HEAD_ICON_RSP = 1032,
	ID_DOWN_LOAD_FILE_REQ = 1033,
	ID_DOWN_LOAD_FILE_RSP = 1034,

	ID_IMG_CHAT_UPLOAD_REQ = 1037,
	ID_IMG_CHAT_UPLOAD_RSP = 1038,

	ID_FILE_INFO_SYNC_REQ = 1041,
	ID_FILE_INFO_SYNC_RSP = 1042,
	ID_IMG_CHAT_CONTINUE_UPLOAD_REQ = 1043,  //续传聊天图片资源请求
	ID_IMG_CHAT_CONTINUE_UPLOAD_RSP = 1044,  //续传聊天图片资源回复
};

#define USERIPPREFIX  "uip_"
#define USERTOKENPREFIX  "utoken_"
#define IPCOUNTPREFIX  "ipcount_"
#define USER_BASE_INFO "ubaseinfo_"
#define LOGIN_COUNT  "logincount"
#define NAME_INFO  "nameinfo_"

#define USERIPPREFIX  "uip_"
#define USERTOKENPREFIX  "utoken_"
#define IPCOUNTPREFIX  "ipcount_"
#define USER_BASE_INFO "ubaseinfo_"
#define LOGIN_COUNT  "logincount"
#define NAME_INFO  "nameinfo_"
#define LOCK_PREFIX "lock_"
#define USERSESSIONPREFIX "usession_"
#define LOCK_COUNT "lockcount"

//分布式锁的持有时间
#define LOCK_TIME_OUT 10
//分布式锁的重试时间
#define ACQUIRE_TIME_OUT 5
