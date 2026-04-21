#ifndef FILETCPMGR_H
#define FILETCPMGR_H

#include <QObject>
#include <singleton.h>
#include <QQueue>
#include <QByteArray>
#include <QThread>
#include <vector>

struct SendTask {
    int file_id;        // 文件唯一标识
    int chunk_id;       // 分片序号
    int total_chunks;   // 总分片数
    std::vector<char> data;  // 数据内容
    int retry_count;    // 重试次数
};

class FileTcpThread: public std::enable_shared_from_this<FileTcpThread>{
public:
    FileTcpThread();
    ~FileTcpThread();
private:
    QThread * _file_tcp_thread;

};

class FileTcpMgr : public QObject, public Singleton<FileTcpMgr>,
                   public std::enable_shared_from_this<FileTcpMgr>
{
    Q_OBJECT
public:
    friend class Singleton<FileTcpMgr>;
    ~FileTcpMgr();
    void SendData(ReqId reqId, QByteArray data);
    void SendDownloadInfo(std::shared_ptr<DownloadInfo> download);
    void CloseConnection();
    void BatchSend(std::shared_ptr<MsgInfo> msg_info);
    void ContinueUploadFile(QString unique_name);
private:
    void initHandlers();
    explicit FileTcpMgr(QObject *parent = nullptr);

    void registerMetaType();
    void handleMsg(ReqId id, int len, QByteArray data);

    QTcpSocket _socket;
    QString _host;
    uint16_t _port;
    QByteArray _buffer;
    bool _b_recv_pending;
    quint16 _message_id;
    quint32 _message_len;
    QMap<ReqId, std::function<void(ReqId id, int len, QByteArray data)>> _handlers;
    //发送队列
    QQueue<QByteArray> _send_queue;
    //正在发送的包
    QByteArray  _current_block;
    //当前已发送的字节数
    qint64        _bytes_sent;
    //是否正在发送
    bool _pending;
    //发送的拥塞窗口，控制发送数量
    int _cwnd_size;
signals:
    void sig_send_data(ReqId reqId, QByteArray data);
    void sig_con_success(bool bsuccess);
    void sig_connection_closed();
    void sig_close();
    void sig_reset_label_icon(QString path);
    void sig_continue_upload_file(QString unique_name);
    void sig_update_upload_progress(std::shared_ptr<MsgInfo> msg_info);
public slots:
    void slot_send_data(ReqId reqId, QByteArray data);
    void slot_tcp_connect(std::shared_ptr<ServerInfo> si);
    void slot_tcp_close();
    void slot_continue_upload_file(QString unique_name);
};

#endif // FILETCPMGR_H
