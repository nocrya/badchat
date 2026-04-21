#ifndef USERDATA_H
#define USERDATA_H
#include <QString>
#include <memory>
#include <vector>
#include <QJsonArray>
#include <QJsonObject>
#include "global.h"

class SearchInfo {
public:
    SearchInfo(int uid, QString name, QString nick, QString desc, int sex, QString icon);
    SearchInfo() = default;
    int _uid;
    QString _name;
    QString _nick;
    QString _desc;
    int _sex;
    QString _icon;
};

Q_DECLARE_METATYPE(SearchInfo)
Q_DECLARE_METATYPE(std::shared_ptr<SearchInfo>)

class AddFriendApply {
public:
    AddFriendApply(int from_uid, QString name, QString desc,
                   QString icon, QString nick, int sex);
    AddFriendApply() = default;
    int _from_uid;
    QString _name;
    QString _desc;
    QString _icon;
    QString _nick;
    int     _sex;
};

Q_DECLARE_METATYPE(AddFriendApply)
Q_DECLARE_METATYPE(std::shared_ptr<AddFriendApply>)

struct ApplyInfo {
    ApplyInfo(int uid, QString name, QString desc,
              QString icon, QString nick, int sex, int status)
        :_uid(uid),_name(name),_desc(desc),
        _icon(icon),_nick(nick),_sex(sex),_status(status){}

    ApplyInfo(std::shared_ptr<AddFriendApply> addinfo)
        :_uid(addinfo->_from_uid),_name(addinfo->_name),
        _desc(addinfo->_desc),_icon(addinfo->_icon),
        _nick(addinfo->_nick),_sex(addinfo->_sex),
        _status(0)
    {}
    ApplyInfo() = default;
    void SetIcon(QString head){
        _icon = head;
    }
    int _uid;
    QString _name;
    QString _desc;
    QString _icon;
    QString _nick;
    int _sex;
    int _status;
};

Q_DECLARE_METATYPE(ApplyInfo)

class TextChatData;

struct AuthInfo {
    AuthInfo(int uid, QString name,
             QString nick, QString icon, int sex):
        _uid(uid), _name(name), _nick(nick), _icon(icon),
        _sex(sex), _thread_id(0) {}
    AuthInfo() = default;
    int _uid;
    QString _name;
    QString _nick;
    QString _icon;
    int _sex;
    int _thread_id;
    std::vector<std::shared_ptr<TextChatData>> _chat_datas;
    void SetChatDatas(std::vector<std::shared_ptr<TextChatData>> chat_datas);
};

Q_DECLARE_METATYPE(AuthInfo)

struct AuthRsp {
    AuthRsp(int peer_uid, QString peer_name,
            QString peer_nick, QString peer_icon, int peer_sex)
        :_uid(peer_uid),_name(peer_name),_nick(peer_nick),
        _icon(peer_icon),_sex(peer_sex), _thread_id(0) {}
    AuthRsp() = default;
    int _uid;
    QString _name;
    QString _nick;
    QString _icon;
    int _sex;
    int _thread_id;
    std::vector<std::shared_ptr<TextChatData>> _chat_datas;
    void SetChatDatas(std::vector<std::shared_ptr<TextChatData>> chat_datas);
};

Q_DECLARE_METATYPE(AuthRsp)
Q_DECLARE_METATYPE(std::shared_ptr<AuthRsp>)


struct UserInfo {
    UserInfo(int uid, QString name, QString nick, QString icon, int sex, QString last_msg = "", QString desc=""):
        _uid(uid),_name(name),_nick(nick),_icon(icon),_sex(sex), _desc(desc){}

    UserInfo(std::shared_ptr<AuthInfo> auth):
        _uid(auth->_uid),_name(auth->_name),_nick(auth->_nick),
        _icon(auth->_icon),_sex(auth->_sex),_desc(""){}

    UserInfo(int uid, QString name, QString icon):
        _uid(uid), _name(name), _icon(icon),_nick(_name),
        _sex(0), _desc(""){

    }

    UserInfo(std::shared_ptr<AuthRsp> auth):
        _uid(auth->_uid),_name(auth->_name),_nick(auth->_nick),
        _icon(auth->_icon),_sex(auth->_sex),_desc(""){}

    UserInfo(std::shared_ptr<SearchInfo> search_info):
        _uid(search_info->_uid),_name(search_info->_name),_nick(search_info->_nick),
        _icon(search_info->_icon),_sex(search_info->_sex),_desc(""){

    }

    UserInfo() = default;
    int _uid;
    QString _name;
    QString _nick;
    QString _icon;
    int _sex;
    QString _desc;
    std::vector<std::shared_ptr<TextChatData>> _chat_msgs;
};

Q_DECLARE_METATYPE(UserInfo)

// struct TextChatData{
//     TextChatData(QString msg_id, QString msg_content, int fromuid, int touid)
//         :_msg_id(msg_id),_msg_content(msg_content),_from_uid(fromuid),_to_uid(touid){

//     }
//     QString _msg_id;
//     QString _msg_content;
//     int _from_uid;
//     int _to_uid;
// };

class ChatDataBase {
public:
    ChatDataBase(int msg_id, int thread_id, ChatFormType form_type, ChatMsgType msg_type,
                 QString content,int send_uid, int status, QString chat_time);
    ChatDataBase(QString unique_id, int thread_id, ChatFormType form_type, ChatMsgType msg_type,
                 QString content, int send_uid, int status, QString chat_time);
    ChatDataBase(int msg_id, QString unique_id, int thread_id, ChatFormType form_type, ChatMsgType msg_type,
                 QString content, int send_uid, int status, QString chat_time);
    int GetMsgId() { return _msg_id; }
    int GetThreadId() { return _thread_id; }
    ChatFormType GetFormType() { return _form_type; }
    ChatMsgType GetMsgType() { return _msg_type; }
    QString GetContent() { return _content; }
    int GetSendUid() { return _send_uid; }
    QString GetMsgContent(){return _content;}
    void SetUniqueId(QString unique_id);
    QString GetUniqueId() {return _unique_id; };
    int GetStatus() { return _status; };
    void SetMsgId(int msg_id) { _msg_id = msg_id; }
    void SetStatus(int status) { _status = status; }
    virtual ~ChatDataBase() {} // 添加虚析构函数，使其成为多态类型
private:
    //聊天唯一id
    QString _unique_id;
    //消息id
    int _msg_id;
    //聊天id
    int _thread_id;
    //聊天类型
    ChatFormType _form_type;
    //消息类型
    ChatMsgType _msg_type;
    //消息内容
    QString _content;
    //发送者id
    int _send_uid;
    //消息状态
    int _status;
    //消息时间
    QString _chat_time;
};


class TextChatData : public ChatDataBase {
public:

    TextChatData(int msg_id, int thread_id, ChatFormType form_type, ChatMsgType msg_type,  QString content,
                 int send_uid, int status, QString chat_time = ""): ChatDataBase(msg_id, thread_id, form_type, msg_type, content, send_uid, status, chat_time)
        //_msg_id(msg_id), _thread_id(thread_id), _form_type(form_type), _msg_type(msg_type), _content(content), _send_uid(send_uid), _status(status)
    {

    }

    TextChatData(QString unique_id, int thread_id, ChatFormType form_type, ChatMsgType msg_type, QString content,
                 int send_uid, int status, QString chat_time = ""): ChatDataBase(unique_id, thread_id, form_type, msg_type, content, send_uid, status, chat_time)
        //_unique_id(unique_id), _thread_id(thread_id), _form_type(form_type), _msg_type(msg_type), _content(content), _send_uid(send_uid), _status(status)
    {

    }

    TextChatData(int msg_id, QString unique_id, int thread_id, ChatFormType form_type, ChatMsgType msg_type,  QString content,
                 int send_uid, int status, QString chat_time = ""): ChatDataBase(msg_id, unique_id, thread_id, form_type, msg_type, content, send_uid, status, chat_time)
        //_msg_id(msg_id), _unique_id(unique_id), _thread_id(thread_id), _form_type(form_type), _msg_type(msg_type), _content(content), _send_uid(send_uid), _status(status)
    {

    }

    TextChatData() = default;

};


Q_DECLARE_METATYPE(std::vector<std::shared_ptr<TextChatData>>)

class ImgChatData : public ChatDataBase {
public:
    ImgChatData(std::shared_ptr<MsgInfo> msg_info, QString unique_id,
                int thread_id, ChatFormType form_type, ChatMsgType msg_type,
                int send_uid, int status, QString chat_time = ""):
        ChatDataBase(unique_id,thread_id, form_type, msg_type, msg_info->_text_or_url,
                     send_uid, status, chat_time), _msg_info(msg_info){

    }

    std::shared_ptr<MsgInfo> _msg_info;
};

Q_DECLARE_METATYPE(std::shared_ptr<ImgChatData>)

//聊天信息结构
struct ChatThreadInfo {
    ChatThreadInfo() = default;
    ChatThreadInfo(const ChatThreadInfo& other):_thread_id(other._thread_id),_type(other._type),_user1_id(other._user1_id),_user2_id(other._user2_id){}
    int _thread_id;
    QString _type;
    int _user1_id;
    int _user2_id;
};

Q_DECLARE_METATYPE(std::vector<std::shared_ptr<ChatThreadInfo>>)

//聊天数据结构
class ChatThreadData {
public:
    ChatThreadData(int other_id, int thread_id, int last_msg_id):
        _other_id(other_id), _thread_id(thread_id), _last_msg_id(last_msg_id){}
    ChatThreadData() = default;
    void AddMsg(std::shared_ptr<ChatDataBase> msg);
    void MoveMsg(std::shared_ptr<ChatDataBase> msg);
    void SetLastMsgId(int msg_id);
    void SetOtherId(int other_id);
    int  GetOtherId();
    QString GetGroupName();
    QMap<int, std::shared_ptr<ChatDataBase>> GetMsgMap();
    int  GetThreadId();
    QMap<int, std::shared_ptr<ChatDataBase>>&  GetMsgMapRef();
    void AppendMsg(int msg_id, std::shared_ptr<ChatDataBase> base_msg);
    QString GetLastMsg();
    int GetLastMsgId();
    QMap<QString, std::shared_ptr<ChatDataBase>>& GetMsgUnRspRef();
    void AppendUnRspMsg(QString unique_id, std::shared_ptr<ChatDataBase> base_msg);
    void UpdateProgress(std::shared_ptr<MsgInfo> msg);
private:
    //对方id 如果是群则为0
    int _other_id;
    int _last_msg_id;
    int _thread_id;
    QString _last_msg;
    //群成员列表
    std::vector<int> _group_members;
    //群名称
    QString _group_name;
    //消息map 类型为基类，实际为文本、图片、文件等消息
    QMap<int, std::shared_ptr<ChatDataBase>>  _msg_map;
    //未收到应答的消息
    //已发送但未收到应答的消息
    QMap<QString, std::shared_ptr<ChatDataBase>> _msg_unrsp_map;
};

Q_DECLARE_METATYPE(std::shared_ptr<ChatThreadInfo>)

#endif // USERDATA_H
