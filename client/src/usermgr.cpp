#include "usermgr.h"
#include "global.h"

UserMgr::UserMgr(): _user_info(nullptr), _chat_loaded(0), _contact_loaded(0), _last_chat_thread_id(0), _cur_load_chat_index(0) {

}

UserMgr::~UserMgr()
{

}

void UserMgr::SetToken(QString token)
{
    std::lock_guard<std::mutex> lock(_mtx);
    _token = token;
}

int UserMgr::GetUid()
{
    std::lock_guard<std::mutex> lock(_mtx);
    return _user_info->_uid;
}

QString UserMgr::GetName()
{
    std::lock_guard<std::mutex> lock(_mtx);
    return _user_info->_name;
}

QString UserMgr::GetIcon()
{
    std::lock_guard<std::mutex> lock(_mtx);
    return _user_info->_icon;
}

QString UserMgr::GetNick()
{
    std::lock_guard<std::mutex> lock(_mtx);
    return _user_info->_nick;
}

QString UserMgr::GetToken()
{
    std::lock_guard<std::mutex> lock(_mtx);
    return _token;
}

QString UserMgr::GetDesc()
{
    std::lock_guard<std::mutex> lock(_mtx);
    return _user_info->_desc;
}

std::shared_ptr<UserInfo> UserMgr::GetUserInfo()
{
    std::lock_guard<std::mutex> lock(_mtx);
    return _user_info;
}

std::vector<std::shared_ptr<ApplyInfo> > UserMgr::GetApplyList()
{
    std::lock_guard<std::mutex> lock(_mtx);
    return _apply_list;
}

bool UserMgr::AlreadyApply(int uid)
{
    for(auto& apply: _apply_list)
    {
        if(apply->_uid == uid)
            return true;
    }

    return false;
}

void UserMgr::AddApplyList(std::shared_ptr<ApplyInfo> apply)
{
    std::lock_guard<std::mutex> lock(_mtx);
    _apply_list.push_back(apply);
}

void UserMgr::SetUserInfo(std::shared_ptr<UserInfo> user_info)
{
    std::lock_guard<std::mutex> lock(_mtx);
    _user_info = user_info;
}

void UserMgr::AppendApplyList(QJsonArray array)
{
    //遍历 QJsonArrry 并输出每个元素
    for(const QJsonValue& value: array)
    {
        auto name = value["name"].toString();
        auto desc = value["desc"].toString();
        auto icon = value["icon"].toString();
        auto nick = value["nick"].toString();
        auto sex = value["sex"].toInt();
        auto uid = value["uid"].toInt();
        auto status = value["status"].toInt();
        auto info = std::make_shared<ApplyInfo>(uid, name, desc, icon, nick, sex, status);

        _apply_list.push_back(info);
    }
}

void UserMgr::AppendFriendList(QJsonArray array)
{
    //遍历 QJsonArrry 并输出每个元素
    for(const QJsonValue& value: array)
    {
        auto name = value["name"].toString();
        auto desc = value["desc"].toString();
        auto icon = value["icon"].toString();
        auto nick = value["nick"].toString();
        auto sex = value["sex"].toInt();
        auto uid = value["uid"].toInt();
        auto back = value["back"].toString();
        auto info = std::make_shared<UserInfo>(uid, name, nick, icon, sex, desc, back);

        _friend_list.push_back(info);
        _friend_map.insert(uid, info);
    }
}

bool UserMgr::CheckFriendById(int uid)
{
    auto iter = _friend_map.find(uid);
    if(iter == _friend_map.end())
    {
        return  false;
    }

    return true;
}

void UserMgr::AddFriend(std::shared_ptr<AuthRsp> auth_rsp)
{
    std::lock_guard<std::mutex> lock(_mtx);
    auto friend_info = std::make_shared<UserInfo>(auth_rsp);
    _friend_map[friend_info->_uid] = friend_info;
}

void UserMgr::AddFriend(std::shared_ptr<AuthInfo> auth_info)
{
    std::lock_guard<std::mutex> lock(_mtx);
    auto friend_info = std::make_shared<UserInfo>(auth_info);
    _friend_map[friend_info->_uid] = friend_info;
}

void UserMgr::SlotAddFriendRsp(std::shared_ptr<AuthRsp> rsp)
{
    AddFriend(rsp);
}

void UserMgr::SlotAddFriendAuth(std::shared_ptr<AuthInfo> auth)
{
    AddFriend(auth);
}

std::shared_ptr<UserInfo> UserMgr::GetFriendById(int uid)
{
    std::lock_guard<std::mutex> lock(_mtx);
    auto find_it = _friend_map.find(uid);
    if(find_it == _friend_map.end()){
        return nullptr;
    }

    return *find_it;
}

void UserMgr::SetLastChatThreadId(int id)
{
    std::lock_guard<std::mutex> lock(_mtx);
    _last_chat_thread_id = id;
}

int UserMgr::GetLastChatThreadId()
{
    std::lock_guard<std::mutex> lock(_mtx);
    return _last_chat_thread_id;
}

void UserMgr::AddChatThreadData(std::shared_ptr<ChatThreadData> chat_thread_data, int other_id)
{
    std::lock_guard<std::mutex> lock(_mtx);
    //建立会话id到数据的映射
    _chat_map[chat_thread_data->GetThreadId()] = chat_thread_data;
    //存储会话列表
    _chat_thread_ids.push_back(chat_thread_data->GetThreadId());
    if (other_id) {
        //将对方uid和会话id关联
        _uid_to_thread_id[other_id] = chat_thread_data->GetThreadId();
    }
}

int UserMgr::GetThreadIdByUid(int uid)
{
    std::lock_guard<std::mutex> lock(_mtx);
    auto find_it = _uid_to_thread_id.find(uid);
    if (find_it == _uid_to_thread_id.end()) {
        return -1;
    }

    return find_it.value();
}

std::shared_ptr<ChatThreadData> UserMgr::GetChatThreadByThreadId(int thread_id)
{
    std::lock_guard<std::mutex> lock(_mtx);
    auto find_it = _chat_map.find(thread_id);
    if (find_it == _chat_map.end()) {
        return nullptr;
    }

    return find_it.value();
}

std::shared_ptr<ChatThreadData> UserMgr::GetChatThreadByUid(int uid)
{
    std::lock_guard<std::mutex> lock(_mtx);
    auto find_it = _chat_map.find(_uid_to_thread_id[uid]);
    if (find_it == _chat_map.end()) {
        return nullptr;
    }

    return find_it.value();
}


std::vector<std::shared_ptr<UserInfo>> UserMgr::GetChatListPerPage() {

    std::lock_guard<std::mutex> lock(_mtx);
    std::vector<std::shared_ptr<UserInfo>> friend_list;
    int begin = _chat_loaded;
    int end = begin + CHAT_COUNT_PER_PAGE;

    if (begin >= _friend_list.size()) {
        return friend_list;
    }

    if (end > _friend_list.size()) {
        friend_list = std::vector<std::shared_ptr<UserInfo>>(_friend_list.begin() + begin, _friend_list.end());
        return friend_list;
    }


    friend_list = std::vector<std::shared_ptr<UserInfo>>(_friend_list.begin() + begin, _friend_list.begin()+ end);
    return friend_list;
}


std::vector<std::shared_ptr<UserInfo>> UserMgr::GetConListPerPage() {
    std::lock_guard<std::mutex> lock(_mtx);
    std::vector<std::shared_ptr<UserInfo>> friend_list;
    int begin = _contact_loaded;
    int end = begin + CHAT_COUNT_PER_PAGE;

    if (begin >= _friend_list.size()) {
        return friend_list;
    }

    if (end > _friend_list.size()) {
        friend_list = std::vector<std::shared_ptr<UserInfo>>(_friend_list.begin() + begin, _friend_list.end());
        return friend_list;
    }


    friend_list = std::vector<std::shared_ptr<UserInfo>>(_friend_list.begin() + begin, _friend_list.begin() + end);
    return friend_list;
}

bool UserMgr::IsLoadChatFin() {
    if (_chat_loaded >= _friend_list.size()) {
        return true;
    }

    return false;
}

void UserMgr::UpdateChatLoadedCount() {
    int begin = _chat_loaded;
    int end = begin + CHAT_COUNT_PER_PAGE;

    if (begin >= _friend_list.size()) {
        return ;
    }

    if (end > _friend_list.size()) {
        _chat_loaded = _friend_list.size();
        return ;
    }

    _chat_loaded = end;
}

void UserMgr::UpdateContactLoadedCount() {
    int begin = _contact_loaded;
    int end = begin + CHAT_COUNT_PER_PAGE;

    if (begin >= _friend_list.size()) {
        return;
    }

    if (end > _friend_list.size()) {
        _contact_loaded = _friend_list.size();
        return;
    }

    _contact_loaded = end;
}

bool UserMgr::IsLoadConFin()
{
    if (_contact_loaded >= _friend_list.size()) {
        return true;
    }

    return false;
}

std::shared_ptr<ChatThreadData> UserMgr::GetCurLoadData()
{
    std::lock_guard<std::mutex> lock(_mtx);
    if (_cur_load_chat_index >= _chat_thread_ids.size()) {
        return nullptr;
    }

    auto iter = _chat_map.find(_chat_thread_ids[_cur_load_chat_index]);
    if (iter == _chat_map.end()) {
        return nullptr;
    }

    return iter.value();
}

std::shared_ptr<ChatThreadData> UserMgr::GetNextLoadData()
{
    std::lock_guard<std::mutex> lock(_mtx);
    _cur_load_chat_index++;
    if (_cur_load_chat_index >= _chat_thread_ids.size()) {
        return nullptr;
    }

    auto iter = _chat_map.find(_chat_thread_ids[_cur_load_chat_index]);
    if (iter == _chat_map.end()) {
        return nullptr;
    }

    return iter.value();
}

void UserMgr::AddMD5File(const QString& md5, QFileInfo fileinfo)
{
    // 检查文件是否存在（可选）
    if (!fileinfo.exists()) {
        qWarning() << "AddMD5File: file does not exist:" << fileinfo.absoluteFilePath();
        return;
    }

    // 用 QFileInfo 提供的路径构造我们自己的 FileInfo
    auto fileInfoPtr = std::make_shared<QFileInfo>(md5, fileinfo.absoluteFilePath());

    // 存入 map（加锁保证线程安全）
    //QMutexLocker locker(&_mutex);
    _file_map[md5] = fileInfoPtr;
}

void UserMgr::AddMD5File(const QString& md5, std::shared_ptr<QFileInfo> fileInfo)
{
    if (!fileInfo) {
        qWarning() << "AddMD5File: fileInfo is null";
        return;
    }
    //QMutexLocker locker(&_mutex);
    _file_map[md5] = fileInfo;
}

std::shared_ptr<QFileInfo> UserMgr::GetFileInfoByMD5(QString md5)
{
    auto it = _file_map.find(md5);
    if (it != _file_map.end()) {
        return it.value();
    }
    return nullptr;
}

 void UserMgr::AddNameFile(QString file_name, std::shared_ptr<QFileInfo>fileInfo)
{
     fileInfo->fileName() = file_name;
}

// void UserMgr::AppendFriendChatMsg(int friend_id,std::vector<std::shared_ptr<TextChatData> > msgs)
// {
//     auto find_iter = _friend_map.find(friend_id);
//     if(find_iter == _friend_map.end()){
//         qDebug()<<"append friend uid  " << friend_id << " not found";
//         return;
//     }

//     find_iter.value()->AppendChatMsgs(msgs);
// }

 bool UserMgr::IsDownLoading(QString name) {
     std::lock_guard<std::mutex> lock(_down_load_mtx);
     auto iter = _name_to_download_info.find(name);
     if (iter == _name_to_download_info.end()) {
         return false;
     }

     return true;
 }

 void UserMgr::AddLabelToReset(QString path, QLabel* label)
 {
     auto iter = _path_to_reset_labels.find(path);
     if (iter == _path_to_reset_labels.end()) {
         QList<QLabel*> list;
         list.append(label);
         _path_to_reset_labels.insert(path, list);
         return;
     }

     iter->append(label);
 }

 void UserMgr::ResetLabelIcon(QString path)
 {
     auto iter = _path_to_reset_labels.find(path);
     if(iter == _path_to_reset_labels.end())
     {
         return;
     }

     for(auto ele_iter = iter.value().begin(); ele_iter != iter.value().end(); ele_iter++)
     {
         QPixmap pixmap(path);
         if(!pixmap.isNull())
         {
             QPixmap scalePixmap = pixmap.scaled((*ele_iter)->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
             (*ele_iter)->setPixmap(scalePixmap);
             (*ele_iter)->setScaledContents(true);
         }
         else {
             qWarning() << "无法加载上传的头像：" << path;
         }
     }

     _path_to_reset_labels.erase(iter);
 }

 void UserMgr::AddDownloadFile(QString name,
     std::shared_ptr<DownloadInfo> file_info) 
 {
     std::lock_guard<std::mutex> lock(_down_load_mtx);
     _name_to_download_info[name] = file_info;
 }

 void UserMgr::RmvDownloadFile(QString name)
 {
     std::lock_guard<std::mutex> lock(_down_load_mtx);
     _name_to_download_info.remove(name);
 }

 std::shared_ptr<DownloadInfo> UserMgr::GetDownloadInfo(QString name)
 {
     std::lock_guard<std::mutex> lock(_down_load_mtx);
     auto iter = _name_to_download_info.find(name);
     if(iter == _name_to_download_info.end())
     {
         return nullptr;
     }

     return iter.value();
 }

 void UserMgr::AddTransFile(QString name,
                               std::shared_ptr<MsgInfo> msg_info)
 {
     std::lock_guard<std::mutex> lock(_trans_mtx);
     _name_to_msg_info[name] = msg_info;
 }

 void UserMgr::RmvTransFileByName(QString name)
 {
     std::lock_guard<std::mutex> lock(_trans_mtx);
     auto iter = _name_to_msg_info.find(name);
     if(iter == _name_to_msg_info.end())
     {
         return;
     }

     _name_to_msg_info.erase(iter);
 }

 std::shared_ptr<MsgInfo> UserMgr::GetTransFileByName(QString name)
 {
     std::lock_guard<std::mutex> lock(_trans_mtx);
     auto iter = _name_to_msg_info.find(name);
     if(iter == _name_to_msg_info.end())
     {
         return nullptr;
     }

     return iter.value();
 }

 void UserMgr::PauseTransFileByName(QString name)
 {
     std::lock_guard<std::mutex> lock(_trans_mtx);
     auto iter = _name_to_msg_info.find(name);
     if(iter == _name_to_msg_info.end())
     {
         return;
     }

     iter.value()->_transfer_state = TransferState::Paused;
 }

 void UserMgr::ResumeTransFileByName(QString name)
 {
     std::lock_guard<std::mutex> lock(_trans_mtx);
     auto iter = _name_to_msg_info.find(name);
     if(iter == _name_to_msg_info.end())
     {
         return;
     }

     if(iter.value()->_transfer_type == TransferType::Download)
     {
         iter.value()->_transfer_state = TransferState::Downloading;
         return;
     }
     if(iter.value()->_transfer_type == TransferType::Upload)
     {
         iter.value()->_transfer_state = TransferState::Uploading;
         return;
     }
 }

 bool UserMgr::TransFileIsUploading(QString name)
 {
     std::lock_guard<std::mutex> lock(_trans_mtx);
     auto iter = _name_to_msg_info.find(name);
     if(iter == _name_to_msg_info.end())
     {
         return false;
     }

     if(iter.value()->_transfer_state == TransferState::Uploading)
     {
         return true;
     }

     return false;
 }

 std::shared_ptr<MsgInfo> UserMgr::GetFreeUploadFile()
 {
     std::lock_guard<std::mutex> lock(_trans_mtx);
     if(_name_to_msg_info.isEmpty())
     {
         return nullptr;
     }
     for(auto iter = _name_to_msg_info.begin(); iter != _name_to_msg_info.end(); iter++)
     {
         //只要传输状态不是暂停就返回一个可用的待传输文件
         if((iter.value()->_transfer_state != TransferState::Paused) && (iter.value()->_transfer_type != TransferType::Upload))
        {
             return iter.value();
        }
     }
     return nullptr;
 }

 std::shared_ptr<MsgInfo> UserMgr::GetFreeDownloadFile()
 {
     std::lock_guard<std::mutex> lock(_trans_mtx);
     if(_name_to_msg_info.isEmpty())
     {
         return nullptr;
     }
     for(auto iter = _name_to_msg_info.begin(); iter != _name_to_msg_info.end(); iter++)
     {
         //只要传输状态不是暂停就返回一个可用的待传输文件
         if((iter.value()->_transfer_state != TransferState::Paused) && (iter.value()->_transfer_type != TransferType::Download))
         {
             return iter.value();
         }
     }
     return nullptr;
 }

