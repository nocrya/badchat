#include "adduseritem.h"
#include "ui_adduseritem.h"

AddUserItem::AddUserItem(QWidget *parent)
    : ListItemBase(parent)
    , ui(new Ui::AddUserItem)
{
    ui->setupUi(this);
    SetItemType(ListItemType::ADD_USER_TIP_ITEM);
}

AddUserItem::~AddUserItem()
{
    delete ui;
}

void AddUserItem::SetSearchText(const QString& text)
{
    if (text.isEmpty()) {
        ui->message_tip->setText(QStringLiteral("查找 uid/name"));
    } else {
        ui->message_tip->setText(QStringLiteral("网络查找: ") + text);
    }
}
