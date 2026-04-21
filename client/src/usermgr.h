#ifndef USERMGR_H
#define USERMGR_H
#include <QOBJECT>
#include <memory>
#include "singleton.h"
#include "userdata.h"
#include <QJsonArray>
#include <mutex>
#include <QFile>
#include <QLabel>

struct FileInfo {
    QString _md5;
    QString _filePath;
    QString _fileName;

    inline QString md5() const { return _md5; }
    inline QString filePath() const { return _filePath; }
    inline QString fileName() const { return _fileName; }

    FileInfo(const QString& md5, const QString& filePath)
        : _md5(md5), _filePath(filePath) {
        QFileInfo fi(filePath);
        _fileName = fi.fileName();
    }
};

class UserMgr: public QObject, public Singleton<UserMgr>,
                public std::enable_shared_from_this<UserMgr>
{
    Q_OBJECT
public:
    friend class Singleton<UserMgr>;
    ~ UserMgr();
    void SetToken(QString token);
    void SetUserInfo(std::shared_ptr<UserInfo> user_info);
    int GetUid();
    QString GetName();
    QString GetIcon();
    QString GetNick();
    QString GetToken();
    QString GetDesc();
    std::shared_ptr<UserInfo> GetUserInfo();
    std::vector<std::shared_ptr<ApplyInfo>> GetApplyList();
    bool AlreadyApply(int uid);
    void AddApplyList(std::shared_ptr<ApplyInfo> apply);
    void AppendApplyList(QJsonArray array);
    void AppendFriendList(QJsonArray array);
    bool CheckFriendById(int uid);
    void AddFriend(std::shared_ptr<AuthRsp> auth_rsp);
    void AddFriend(std::shared_ptr<AuthInfo> auth_info);

    std::shared_ptr<UserInfo> GetFriendById(int uid);
    void SetLastChatThreadId(int id);
    int GetLastChatThreadId();
    void AddChatThreadData(std::shared_ptr<ChatThreadData> chat_thread_data, int other_id);
    int GetThreadIdByUid(int uid);
    std::shared_ptr<ChatThreadData> GetChatThreadByThreadId(int thread_id);
    std::shared_ptr<ChatThreadData> GetChatThreadByUid(int uid);


    std::vector<std::shared_ptr<UserInfo>> GetChatListPerPage();
    bool IsLoadChatFin();
    void UpdateChatLoadedCount();
    std::vector<std::shared_ptr<UserInfo>> GetConListPerPage();
    void UpdateContactLoadedCount();
    bool IsLoadConFin();

    //void AppendFriendChatMsg(int friend_id,std::vector<std::shared_ptr<TextChatData> > msgs);
    //获取当前正在加载的聊天数据。
    std::shared_ptr<ChatThreadData> GetCurLoadData();
    std::shared_ptr<ChatThreadData> GetNextLoadData();

    void AddMD5File(const QString& md5, QFileInfo fileinfo);
    void AddMD5File(const QString& md5, std::shared_ptr<QFileInfo> fileInfo);
    std::shared_ptr<QFileInfo> GetFileInfoByMD5(QString md5);
    void AddNameFile(QString file_name, std::shared_ptr<QFileInfo>fileInfo);

    bool IsDownLoading(QString name);
    void AddLabelToReset(QString path, QLabel* label);
    void ResetLabelIcon(QString path);
    void AddDownloadFile(QString name, std::shared_ptr<DownloadInfo> file_info);
    void RmvDownloadFile(QString name);
    std::shared_ptr<DownloadInfo> GetDownloadInfo(QString name);

    void AddTransFile(QString name, std::shared_ptr<MsgInfo> msg_info);
    void RmvTransFileByName(QString name);
    std::shared_ptr<MsgInfo> GetTransFileByName(QString name);

    void PauseTransFileByName(QString name);
    void ResumeTransFileByName(QString name);
    bool TransFileIsUploading(QString name);
    std::shared_ptr<MsgInfo> GetFreeUploadFile();
    std::shared_ptr<MsgInfo> GetFreeDownloadFile();
protected:
    UserMgr();
private:

    QString _token;
    std::vector<std::shared_ptr<ApplyInfo>> _apply_list;
    std::shared_ptr<UserInfo> _user_info;
    QMap<int, std::shared_ptr<UserInfo>> _friend_map;
    std::vector<std::shared_ptr<UserInfo>> _friend_list;
    QMap<int, std::shared_ptr<ChatThreadInfo>> _chat_thread_map;
    QMap<int, std::shared_ptr<ChatThreadData>> _chat_map;
    QMap<int, int>_uid_to_thread_id;
    //聊天会话id列表
    std::vector<int> _chat_thread_ids;
    int _chat_loaded;
    int _contact_loaded;
    int _last_chat_thread_id;
    int _cur_load_chat_index;
    std::mutex _mtx;

    //上传文件md5与文件信息的映射
    QMap<QString, std::shared_ptr<QFileInfo>> _file_map;

    std::mutex _down_load_mtx;
    std::mutex _trans_mtx;
    //名字关联下载信息
    QMap<QString, std::shared_ptr<DownloadInfo>> _name_to_download_info;
    QHash<QString, QList<QLabel*>> _path_to_reset_labels;
    QMap<QString, std::shared_ptr<MsgInfo>> _name_to_msg_info;

public slots:
    void SlotAddFriendRsp(std::shared_ptr<AuthRsp> rsp);
    void SlotAddFriendAuth(std::shared_ptr<AuthInfo> auth);
};

#endif // USERMGR_H
