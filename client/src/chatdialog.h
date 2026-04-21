#ifndef CHATDIALOG_H
#define CHATDIALOG_H

#include <QDialog>
#include "global.h"
#include "statewidget.h"
#include "userdata.h"
#include "loadingdlg.h"
#include <QListWidgetItem>

namespace Ui {
class ChatDialog;
}

class ChatDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChatDialog(QWidget *parent = nullptr);
    ~ChatDialog();
    //void addChatUserList();
    void loadChatList();
    void loadChatMsg();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
    void CloseFindDlg();
    void handleGlobalMousePress(QMouseEvent *event);
    void UpdateChatMsg(std::vector<std::shared_ptr<TextChatData>> msgdata);
private:
    void showLoadingDlg(bool show, QString loading_text);
    void ShowSearch(bool bsearch = false);
    void ClearLabelState(StateWidget* lb);
    void AddLBGroup(StateWidget *lb);
    void SetSelectChatItem(int thread_id = 0);
    void SetSelectChatPage(int thread_id = 0);
    void loadMoreChatUser();
    void loadMoreConUser();

    Ui::ChatDialog *ui;
    ChatUIMode _mode;
    ChatUIMode _state;
    bool _b_loading;
    QList<StateWidget*> _lb_list;
    LoadingDlg* _loading_dlg;
    //QMap<int, QListWidgetItem*> _chat_items_added;
    QMap<int, QListWidgetItem*> _chat_thread_items;
    int _cur_chat_thread_id;
    QWidget* _last_widget;
    QWidget* _empty_page;
    QTimer* _timer;
    LoadingDlg* _loadingDialog;
    std::shared_ptr<ChatThreadData> _cur_load_chat;
private slots:
    void slot_load_chat_thread(bool load_more, int last_thread_id,
        std::vector<std::shared_ptr<ChatThreadInfo>> chat_threads);
    void slot_create_private_chat(int uid, int other_id, int thread_id);
    void slot_load_chat_msg(int thread_id, int msg_id, bool load_more, std::vector<std::shared_ptr<TextChatData>> msglists);
    void slot_add_chat_msg(int thread_id, std::vector<std::shared_ptr<TextChatData>> msglists);
    void slot_add_img_msg(int thread_id, std::shared_ptr<ImgChatData> msgdata);
    void slot_loading_chat_user();
    void slot_side_chat();
    void slot_side_contact();
    void slot_side_settings();
    void slot_text_changed(const QString& str);
    void slot_loading_contact_user();

public slots:
    void slot_apply_friend(std::shared_ptr<AddFriendApply> apply);
    void slot_add_auth_friend(std::shared_ptr<AuthInfo> auth_info);
    void slot_auth_rsp(std::shared_ptr<AuthRsp> auth_rsp);
    void slot_jump_chat_item(std::shared_ptr<SearchInfo> si);
    void slot_friend_info_page(std::shared_ptr<UserInfo> user_info);
    void slot_switch_apply_friend_page();
    void slot_jump_chat_item_from_infopage(std::shared_ptr<UserInfo> user_info);
    void slot_item_clicked(QListWidgetItem *item);
    //void slot_append_send_chat_msg(std::shared_ptr<TextChatData> msgdata);
    void slot_text_chat_msg(std::vector<std::shared_ptr<TextChatData>> msg);
    void slot_reset_icon(QString path);
    void slot_update_upload_progress(std::shared_ptr<MsgInfo> msg_info);
};

#endif // CHATDIALOG_H
