#ifndef GLOBAL_H
#define GLOBAL_H

#include <QWidget>
#include <functional>
#include <QRegularExpression>
#include "QStyle"
#include <memory>
#include <iostream>
#include <mutex>
#include <QByteArray>
#include <QNetworkReply>
#include <QJsonObject>
#include <QDir>
#include <QSettings>
#include <set>


extern std::function<void(QWidget*)> repolish;

extern std::function<QString(QString)> xorString;

enum ReqId{
    ID_GET_VARIFY_CODE = 1001,  //获取验证码
    ID_REG_USER = 1002, //注册用户
    ID_RESET_PWD = 1003, //重置密码
    ID_LOGIN_USER = 1004, //用户登录
    ID_CHAT_LOGIN = 1005, //登陆聊天服务器
    ID_CHAT_LOGIN_RSP= 1006, //登陆聊天服务器回包
    ID_SEARCH_USER_REQ = 1007, //用户搜索请求
    ID_SEARCH_USER_RSP = 1008, //搜索用户回包
    ID_ADD_FRIEND_REQ = 1009,  //添加好友申请
    ID_ADD_FRIEND_RSP = 1010, //申请添加好友回复
    ID_NOTIFY_ADD_FRIEND_REQ = 1011,  //通知用户添加好友申请
    ID_AUTH_FRIEND_REQ = 1013,  //认证好友请求
    ID_AUTH_FRIEND_RSP = 1014,  //认证好友回复
    ID_NOTIFY_AUTH_FRIEND_REQ = 1015, //通知用户认证好友申请
    ID_TEXT_CHAT_MSG_REQ  = 1017,  //文本聊天信息请求
    ID_TEXT_CHAT_MSG_RSP  = 1018,  //文本聊天信息回复
    ID_NOTIFY_TEXT_CHAT_MSG_REQ = 1019, //通知用户文本聊天信息
    ID_NOTIFY_OFF_LINE_REQ = 1021, //通知用户下线
    ID_HEARTBEAT_REQ = 1023,      //心跳请求
    ID_HEARTBEAT_RSP = 1024,       //心跳回复
    ID_LOAD_CHAT_THREAD_REQ = 1025,	//加载聊天线程请求
    ID_LOAD_CHAT_THREAD_RSP = 1026,	//聊天线程回复
    ID_CREATE_PRIVATE_CHAT_REQ = 1027,	//创建聊天表请求
    ID_CREATE_PRIVATE_CHAT_RSP = 1028,	//创建聊天表回复
    ID_LOAD_CHAT_MSG_REQ = 1029,
    ID_LOAD_CHAT_MSG_RSP = 1030,
    ID_UPLOAD_HEAD_ICON_REQ = 1031,
    ID_UPLOAD_HEAD_ICON_RSP = 1032,
    ID_DOWN_LOAD_FILE_REQ = 1033,
    ID_DOWN_LOAD_FILE_RSP = 1034,
    ID_IMG_CHAT_MSG_REQ = 1035,
    ID_IMG_CHAT_MSG_RSP = 1036,
    ID_IMG_CHAT_UPLOAD_REQ = 1037,
    ID_IMG_CHAT_UPLOAD_RSP = 1038,
    ID_NOTIFY_IMG_CHAT_MSG_REQ = 1039,

    ID_FILE_INFO_SYNC_REQ = 1041,
    ID_FILE_INFO_SYNC_RSP = 1042,
    ID_IMG_CHAT_CONTINUE_UPLOAD_REQ = 1043,  //续传聊天图片资源请求
    ID_IMG_CHAT_CONTINUE_UPLOAD_RSP = 1044,  //续传聊天图片资源回复

};

Q_DECLARE_METATYPE(ReqId)

enum Modules{
    REGISTERMOD = 0,
    RESETMOD = 1,
    LOGINMOD = 2
};

enum ClickLbState{
    Normal = 0,
    Selected = 1
};

enum TipErr{
    TIP_SUCCESS = 0,
    TIP_EMAIL_ERR = 1,
    TIP_PWD_ERR = 2,
    TIP_CONFIRM_ERR = 3,
    TIP_PWD_CONFIRM = 4,
    TIP_VARIFY_ERR = 5,
    TIP_USER_ERR = 6
};

enum ErrorCodes{
    SUCCESS = 0,
    ERR_JSON = 1,   //json解析失败
    ERR_NETWORK = 2,    //网络错误
    PasswdInvalid = 1009,
};

extern QString gate_url_prefix;

struct ServerInfo{
    ServerInfo() = default;
    ServerInfo(const ServerInfo& other):_chat_host(other._chat_host),_chat_port(other._chat_port),_token(other._token),_uid(other._uid){}
    QString _chat_host;
    QString _chat_port;
    QString _res_host;
    QString _res_port;
    QString _token;
    int _uid;
};

Q_DECLARE_METATYPE(ServerInfo)
Q_DECLARE_METATYPE(std::shared_ptr<ServerInfo>);

//聊天界面不同模式
enum ChatUIMode{
    SearchMode, //搜索模式
    ChatMode,   //聊天模式
    ContactMode,    //联系模式
    SettingMode,
};

//自定义QListWidgetItem的几种类型
enum ListItemType{
    CHAT_USER_ITEM, //聊天用户
    CONTACT_USER_ITEM, //联系人用户
    SEARCH_USER_ITEM, //搜索到的用户
    ADD_USER_TIP_ITEM, //提示添加用户
    INVALID_ITEM,  //不可点击条目
    GROUP_TIP_ITEM, //分组提示条目
    LINE_ITEM,  //分割线
    APPLY_FRIEND_ITEM, //好友申请
};

enum class ChatRole
{
    Self,
    Other
};

enum MsgType {
    TEXT_MSG,
    IMG_MSG,
    FILE_MSG
};

enum TransferType {
    None,
    Upload,
    Download,
};

enum class TransferState {
    None,
    Downloading,
    Uploading,
    Paused,
    Failed,
    Completed
};

//申请好友标签输入框最低长度
const int MIN_APPLY_LABEL_ED_LEN = 40;

//TCP包头长度
#define TCP_HEAD_LEN 6
//TCP ID长度
#define TCP_ID_LEN 2
//TCP 长度字段的长度
#define TCP_LEN_LEN 4
//最大文件长度
#define MAX_FILE_LEN 1024*32

#define FILE_UPLOAD_HEAD_LEN 6

#define MAX_CWND_SIZE 5

struct MsgInfo{
    MsgInfo() = default;
    MsgInfo(MsgType msgtype, QString text_or_url, QPixmap pixmap, QString unique_name, qint64 total_size, QString md5)
        :_msg_type(msgtype), _text_or_url(text_or_url), _preview_pix(pixmap),_unique_name(unique_name),_total_size(total_size),
        _current_size(0),_seq(1),_md5(md5), _last_confirmed_seq(0), _rsp_size(0), _transfer_type(TransferType::None), _transfer_state(TransferState::None)
    {
        _max_seq = (total_size + MAX_FILE_LEN - 1) / MAX_FILE_LEN;
    }

    MsgType _msg_type;
    QString _text_or_url;//表示文件和图像的url,文本信息
    QPixmap _preview_pix;//文件和图片的缩略图
    QString _unique_name; //文件唯一名字
    qint64 _total_size; //文件总大小
    qint64 _current_size; //传输大小
    qint64 _seq;          //传输序号
    QString _md5;         //文件md5
    std::set<qint64> _rsp_seqs;
    std::set<qint64> _flighting_seqs;
    qint64 _last_confirmed_seq;
    qint64 _max_seq;
    qint64 _msg_id;
    qint64 _rsp_size;
    qint64 _thread_id;
    TransferType _transfer_type;
    TransferState _transfer_state;
};

Q_DECLARE_METATYPE(MsgInfo)
Q_DECLARE_METATYPE(std::shared_ptr<MsgInfo>);

const QString add_prefix = "添加标签 ";

const int  tip_offset = 5;

// inline std::vector<QString>  strs ={"hello world !",
//                              "nice to meet u",
//                              "New year，new life",
//                              "You have to love yourself",
//                              "My love is written in the wind ever since the whole world is you"};

// inline std::vector<QString> heads = {
//     ":/res/head_1.jpg",
//     ":/res/head_2.jpg",
//     ":/res/head_3.jpg",
//     ":/res/head_4.jpg",
//     ":/res/head_1.jpeg",
//     ":/res/head.jpg",
//     ":/res/head2.png"
// };

// inline std::vector<QString> names = {
//     "llfc",
//     "zack",
//     "golang",
//     "cpp",
//     "java",
//     "nodejs",
//     "python",
//     "rust"
// };

const int CHAT_COUNT_PER_PAGE = 13;

enum MsgStatus {
    UN_READ = 0,    //未读
    SEND_FAILED = 1,
    READED = 2
};

enum class ChatFormType {
    PRIVATE,
    GROUP
};

enum class ChatMsgType {
    TEXT,
    IMAGE,
    FILE
};

struct DownloadInfo {
    QString _name;
    int _total_size;
    int _current_size;
    int _seq;
    QString _client_path;
};



#endif // GLOBAL_H
