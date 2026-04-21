#ifndef CHATPAGE_H
#define CHATPAGE_H

#include <QWidget>
#include "userdata.h"
#include "chatitembase.h"

namespace Ui {
class ChatPage;
}

class ChatPage : public QWidget
{
    Q_OBJECT

public:
    explicit ChatPage(QWidget *parent = nullptr);
    ~ChatPage();
    void SetChatData(std::shared_ptr<ChatThreadData> chat_data);
    void AppendChatMsg(std::shared_ptr<ChatDataBase> msg);
    void UpdateChatStatus(QString unique_id, int status);
    void clearItems();
    void UpdateFileProgress(std::shared_ptr<MsgInfo> msg_info);

protected:
    void paintEvent(QPaintEvent *event);

private slots:
    void on_send_btn_clicked();
    void on_receive_btn_clicked();
    void on_clicked_paused(QString unique_name, TransferType transfer_type);
    void on_clicked_resume(QString unique_name, TransferType transfer_type);
private:
    Ui::ChatPage *ui;
    std::shared_ptr<ChatThreadData> _chat_data;
    QHash<QString, ChatItemBase*>_unrsp_item_map;
    QHash<qint64, ChatItemBase*>_base_item_map;
signals:
    void sig_append_send_chat_msg(std::shared_ptr<TextChatData> msg);
};

#endif // CHATPAGE_H
