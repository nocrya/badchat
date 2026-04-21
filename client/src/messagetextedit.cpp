#include "MessageTextEdit.h"
#include <QDebug>
#include <QMessageBox>
#include "chatitembase.h"
#include "textbubble.h"
#include <QJsonDocument>
#include <random>
#include "tcpmgr.h"
#include "usermgr.h"
#include "picturebubble.h"
#include "ui_chatpage.h"

MessageTextEdit::MessageTextEdit(QWidget *parent)
    : QTextEdit(parent), ui(new Ui::ChatPage)
{

    //this->setStyleSheet("border: none;");
    this->setMaximumHeight(60);

    //    connect(this,SIGNAL(textChanged()),this,SLOT(textEditChanged()));

}

MessageTextEdit::~MessageTextEdit()
{

}

QVector<std::shared_ptr<MsgInfo>> MessageTextEdit::getMsgList()
{
    _total_msg_list.clear();

    QString doc = this->document()->toPlainText();
    QString text="";//存储文本信息
    int indexUrl = 0;
    //存储的富文本信息
    int count = _img_or_file_list.size();

    for(int index=0; index<doc.size(); index++)
    {
        //遇到替换符，说明后面是图片或者文件的url
        if(doc[index]==QChar::ObjectReplacementCharacter)
        {
            if(!text.isEmpty())
            {
                //QPixmap pix;
                insertMsgList(_total_msg_list, MsgType::TEXT_MSG, text, QPixmap(), "", 0, "");
                text.clear();
            }
            while(indexUrl < count)
            {
                std::shared_ptr<MsgInfo> msg =  _img_or_file_list[indexUrl];
                if(this->document()->toHtml().contains(msg->_text_or_url, Qt::CaseSensitive))
                {
                    indexUrl++;
                    _total_msg_list.append(msg);
                    break;
                }
                indexUrl++;
            }
        }
        else
        {
            text.append(doc[index]);
        }
    }
    if(!text.isEmpty())
    {
        //QPixmap pix;
        insertMsgList(_total_msg_list, MsgType::TEXT_MSG, text, QPixmap(), "", 0, "");
        text.clear();
    }
    _img_or_file_list.clear();
    this->clear();
    return _total_msg_list;
}

// void MessageTextEdit::send()
// {
//     if (_user_info == nullptr) {
//         qDebug() << "friend_info is empty";
//         return;
//     }

//     auto user_info = UserMgr::GetInstance()->GetUserInfo();
//     auto pTextEdit = ui->chatEdit;
//     ChatRole role = ChatRole::Self;
//     QString userName = user_info->_name;
//     QString userIcon = user_info->_icon;

//     const QVector<MsgInfo>& msgList = pTextEdit->getMsgList();
//     QJsonObject textObj;
//     QJsonArray textArray;
//     int txt_size = 0;

//     for(int i=0; i<msgList.size(); ++i)
//     {
//         //消息内容长度不合规就跳过
//         if(msgList[i].content.length() > 1024){
//             continue;
//         }

//         QString type = msgList[i].msgFlag;
//         ChatItemBase *pChatItem = new ChatItemBase(role);
//         pChatItem->setUserName(userName);
//         pChatItem->setUserIcon(QPixmap(userIcon));
//         QWidget *pBubble = nullptr;

//         if(type == "text")
//         {
//             //生成唯一id
//             QUuid uuid = QUuid::createUuid();
//             //转为字符串
//             QString uuidString = uuid.toString();

//             pBubble = new TextBubble(role, msgList[i].content);
//             if(txt_size + msgList[i].content.length()> 1024){
//                 textObj["fromuid"] = user_info->_uid;
//                 textObj["touid"] = _user_info->_uid;
//                 textObj["text_array"] = textArray;
//                 QJsonDocument doc(textObj);
//                 QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
//                 //发送并清空之前累计的文本列表
//                 txt_size = 0;
//                 textArray = QJsonArray();
//                 textObj = QJsonObject();
//                 //发送tcp请求给chat server
//                 emit TcpMgr::GetInstance()->sig_send_data(ReqId::ID_TEXT_CHAT_MSG_REQ, jsonData);
//             }

//             //将bubble和uid绑定，以后可以等网络返回消息后设置是否送达
//             //_bubble_map[uuidString] = pBubble;
//             txt_size += msgList[i].content.length();
//             QJsonObject obj;
//             QByteArray utf8Message = msgList[i].content.toUtf8();
//             obj["content"] = QString::fromUtf8(utf8Message);
//             obj["msgid"] = uuidString;
//             textArray.append(obj);
//             auto txt_msg = std::make_shared<TextChatData>(uuidString, obj["content"].toString(),
//                                                           user_info->_uid, _user_info->_uid);
//             emit sig_append_send_chat_msg(txt_msg);
//         }
//         else if(type == "image")
//         {
//             pBubble = new PictureBubble(QPixmap(msgList[i].content) , role);
//         }
//         else if(type == "file")
//         {

//         }
//         //发送消息
//         if(pBubble != nullptr)
//         {
//             pChatItem->setWidget(pBubble);
//             ui->chat_data_list->appendChatItem(pChatItem);
//         }

//     }

//     qDebug() << "textArray is " << textArray ;
//     //发送给服务器
//     textObj["text_array"] = textArray;
//     textObj["fromuid"] = user_info->_uid;
//     textObj["touid"] = _user_info->_uid;
//     QJsonDocument doc(textObj);
//     QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
//     //发送并清空之前累计的文本列表
//     txt_size = 0;
//     textArray = QJsonArray();
//     textObj = QJsonObject();
//     //发送tcp请求给chat server
//     emit TcpMgr::GetInstance()->sig_send_data(ReqId::ID_TEXT_CHAT_MSG_REQ, jsonData);
// }

void MessageTextEdit::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->source()==this)
        event->ignore();
    else
        event->accept();
}

void MessageTextEdit::dropEvent(QDropEvent *event)
{
    insertFromMimeData(event->mimeData());
    event->accept();
}

void MessageTextEdit::keyPressEvent(QKeyEvent *e)
{
    if((e->key()==Qt::Key_Enter||e->key()==Qt::Key_Return)&& !(e->modifiers() & Qt::ShiftModifier))
    {
        emit send();
        return;
    }
    QTextEdit::keyPressEvent(e);
}

void MessageTextEdit::insertFileFromUrl(const QStringList &urls)
{
    if(urls.isEmpty())
        return;

    foreach (QString url, urls){
        if(isImage(url))
            insertImages(url);
        else
            insertFiles(url);
    }
}

QString generateUniqueFileName(const QString &originName)
{
    QFileInfo fileInfo(originName);
    QString baseName = fileInfo.completeBaseName();   // 文件名（不含扩展名）
    QString suffix   = fileInfo.suffix();             // 扩展名（如 jpg, png）

    // 1. 使用 C++ 标准库生成随机数
    std::random_device rd;  // 用于生成种子的设备
    std::mt19937 gen(rd()); // Mersenne Twister 算法生成器
    std::uniform_int_distribution<> dis(0, 9999); // 生成 0-9999 的整数

    // 2. 组合时间戳和随机数
    QString uniqueSuffix = QString("%1_%2")
                               .arg(QDateTime::currentMSecsSinceEpoch())
                               .arg(dis(gen)); // 使用 C++ 随机数代替 qrand()
    // 组合为唯一文件名
    return QString("%1_%2.%3").arg(baseName).arg(uniqueSuffix).arg(suffix);
}

QString calculateFileHash(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return QString(); // 打开失败返回空
    }

    QCryptographicHash hash(QCryptographicHash::Md5);
    const int bufferSize = 8192; // 8KB 缓冲区，平衡性能与内存占用
    char buffer[bufferSize];

    // 分块读取文件并更新哈希
    while (!file.atEnd()) {
        qint64 bytesRead = file.read(buffer, bufferSize);
        if (bytesRead > 0) {
            hash.addData(buffer, bytesRead);
        }
    }
    file.close();

    // 返回 32 位小写 MD5 字符串
    return hash.result().toHex();
}

void MessageTextEdit::insertImages(const QString &url)
{
    QFileInfo fileInfo(url);
    if(fileInfo.isDir())
    {
        QMessageBox::information(this, "提示", "只允许拖拽单个文件");
        return;
    }

    auto total_size = fileInfo.size();
    qint64 max_size = qint64(2) * 1024 * 1024 * 1024;

    if(total_size > max_size)
    {
        QMessageBox::information(this, "提示", "发送的文件大小不能大于2G");
        return;
    }

    QString fileMd5 = calculateFileHash(url);

    if (fileMd5.isEmpty())
    {
        QMessageBox::information(this, "提示", "无法计算文件md5");
        return;
    }
    QImage image(url);
    //按比例缩放图片
    if(image.width()>120||image.height()>80)
    {
        if(image.width()>image.height())
        {
            image =  image.scaledToWidth(120,Qt::SmoothTransformation);
        }
        else
            image = image.scaledToHeight(80,Qt::SmoothTransformation);
    }
    QTextCursor cursor = this->textCursor();
    // QTextDocument *document = this->document();
    // document->addResource(QTextDocument::ImageResource, QUrl(url), QVariant(image));
    cursor.insertImage(image,url);
    QString origin_name = fileInfo.fileName();
    QString unique_name = generateUniqueFileName(origin_name);

    insertMsgList(_img_or_file_list, MsgType::IMG_MSG, url, QPixmap::fromImage(image), unique_name, total_size, fileMd5);
}

void MessageTextEdit::insertFiles(const QString &url)
{
    QFileInfo fileInfo(url);
    if(fileInfo.isDir())
    {
        QMessageBox::information(this, "提示", "只允许拖拽单个文件");
        return;
    }

    auto total_size = fileInfo.size();
    qint64 max_size = qint64(2) * 1024 * 1024 * 1024;

    if(total_size > max_size)
    {
        QMessageBox::information(this, "提示", "发送的文件大小不能大于2G");
        return;
    }

    QString fileMd5 = calculateFileHash(url);

    if (fileMd5.isEmpty())
    {
        QMessageBox::information(this, "提示", "无法计算文件md5");
        return;
    }

    QPixmap pix = getFileIconPixmap(url);
    QTextCursor cursor = this->textCursor();
    cursor.insertImage(pix.toImage(), url);

    QString origin_name = fileInfo.fileName();
    QString unique_name = generateUniqueFileName(origin_name);

    insertMsgList(_img_or_file_list, MsgType::FILE_MSG, url, pix, unique_name, total_size, fileMd5);
}

bool MessageTextEdit::canInsertFromMimeData(const QMimeData *source) const
{
    return QTextEdit::canInsertFromMimeData(source);
}

void MessageTextEdit::insertFromMimeData(const QMimeData *source)
{
    QStringList urls = getUrl(source->text());

    if(urls.isEmpty())
        return;

    foreach (QString url, urls)
    {
        if(isImage(url))
            insertImages(url);
        else
            insertFiles(url);
    }
}

bool MessageTextEdit::isImage(QString url)
{
    QString imageFormat = "bmp,jpg,png,tif,gif,pcx,tga,exif,fpx,svg,psd,cdr,pcd,dxf,ufo,eps,ai,raw,wmf,webp";
    QStringList imageFormatList = imageFormat.split(",");
    QFileInfo fileInfo(url);
    QString suffix = fileInfo.suffix();
    if(imageFormatList.contains(suffix,Qt::CaseInsensitive)){
        return true;
    }
    return false;
}

void MessageTextEdit::insertMsgList(QVector<std::shared_ptr<MsgInfo>> &list, MsgType msgtype,
                                    QString text_or_url, QPixmap preview_pix,
                                    QString unique_name, uint64_t total_size, QString md5) {

    auto msg_info = std::make_shared<MsgInfo>(msgtype, text_or_url, preview_pix, unique_name, total_size, md5);
    list.append(msg_info);

}

QStringList MessageTextEdit::getUrl(QString text)
{
    QStringList urls;
    if(text.isEmpty()) return urls;

    QStringList list = text.split("\n");
    foreach (QString url, list) {
        if(!url.isEmpty()){
            QStringList str = url.split("///");
            if(str.size()>=2)
                urls.append(str.at(1));
        }
    }
    return urls;
}

QPixmap MessageTextEdit::getFileIconPixmap(const QString &url)
{
    QFileIconProvider provder;
    QFileInfo fileinfo(url);
    QIcon icon = provder.icon(fileinfo);

    QString strFileSize = getFileSize(fileinfo.size());
    //qDebug() << "FileSize=" << fileinfo.size();

    QFont font(QString("宋体"),10,QFont::Normal,false);
    QFontMetrics fontMetrics(font);
    QSize textSize = fontMetrics.size(Qt::TextSingleLine, fileinfo.fileName());

    QSize FileSize = fontMetrics.size(Qt::TextSingleLine, strFileSize);
    int maxWidth = textSize.width() > FileSize.width() ? textSize.width() :FileSize.width();
    QPixmap pix(50 + maxWidth + 10, 50);
    pix.fill();

    QPainter painter;
    // painter.setRenderHint(QPainter::Antialiasing, true);
    //painter.setFont(font);
    painter.begin(&pix);
    // 文件图标
    QRect rect(0, 0, 50, 50);
    painter.drawPixmap(rect, icon.pixmap(40,40));
    painter.setPen(Qt::black);
    // 文件名称
    QRect rectText(50+10, 3, textSize.width(), textSize.height());
    painter.drawText(rectText, fileinfo.fileName());
    // 文件大小
    QRect rectFile(50+10, textSize.height()+5, FileSize.width(), FileSize.height());
    painter.drawText(rectFile, strFileSize);
    painter.end();
    return pix;
}

QString MessageTextEdit::getFileSize(qint64 size)
{
    QString Unit;
    double num;
    if(size < 1024){
        num = size;
        Unit = "B";
    }
    else if(size < 1024 * 1224){
        num = size / 1024.0;
        Unit = "KB";
    }
    else if(size <  1024 * 1024 * 1024){
        num = size / 1024.0 / 1024.0;
        Unit = "MB";
    }
    else{
        num = size / 1024.0 / 1024.0/ 1024.0;
        Unit = "GB";
    }
    return QString::number(num,'f',2) + " " + Unit;
}

void MessageTextEdit::textEditChanged()
{
    //qDebug() << "text changed!" << endl;
}

