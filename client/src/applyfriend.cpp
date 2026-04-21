#include "applyfriend.h"
#include "ui_applyfriend.h"
#include "usermgr.h"
#include "global.h"
#include <QJsonDocument>
#include "tcpmgr.h"

ApplyFriend::ApplyFriend(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ApplyFriend)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    this->setObjectName("ApplyFriend");
    this->setModal(true);
    ui->apply_ed->setPlaceholderText(QStringLiteral("请求添加你为好友"));
    ui->back_ed->setPlaceholderText(QString());

    ui->sure_btn->SetState("normal","hover","press");
    ui->cancel_btn->SetState("normal","hover","press");
    connect(ui->cancel_btn, &QPushButton::clicked, this, &ApplyFriend::SlotApplyCancel);
    connect(ui->sure_btn, &QPushButton::clicked, this, &ApplyFriend::SlotApplySure);
}

ApplyFriend::~ApplyFriend()
{
    delete ui;
}

void ApplyFriend::SetSearchInfo(std::shared_ptr<SearchInfo> si)
{
    _si = si;
    auto desc = UserMgr::GetInstance()->GetDesc();
    auto bakname = si->_name;
    ui->apply_ed->setText(desc);
    ui->back_ed->setText(bakname);
}

void ApplyFriend::SlotApplyCancel()
{
    this->hide();
    deleteLater();
}

void ApplyFriend::SlotApplySure()
{
    QJsonObject jsonObj;
    auto uid = UserMgr::GetInstance()->GetUid();
    jsonObj["uid"] = uid;
    auto desc = ui->apply_ed->text();
    if(desc.isEmpty()){
        desc = ui->apply_ed->placeholderText();
    }
    jsonObj["desc"] = desc;
    auto bakname = ui->back_ed->text();
    if(bakname.isEmpty()){
        bakname = ui->back_ed->placeholderText();
    }
    jsonObj["bakname"] = bakname;
    jsonObj["touid"] = _si->_uid;

    QJsonDocument doc(jsonObj);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);

    emit TcpMgr::GetInstance()->sig_send_data(ReqId::ID_ADD_FRIEND_REQ, jsonData);

    this->hide();
    deleteLater();
}
