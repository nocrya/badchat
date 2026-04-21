#ifndef APPLYFRIEND_H
#define APPLYFRIEND_H

#include <QDialog>
#include "userdata.h"

namespace Ui {
class ApplyFriend;
}

class ApplyFriend : public QDialog
{
    Q_OBJECT

public:
    explicit ApplyFriend(QWidget *parent = nullptr);
    ~ApplyFriend();
    void SetSearchInfo(std::shared_ptr<SearchInfo> si);

private:
    Ui::ApplyFriend *ui;
    std::shared_ptr<SearchInfo> _si;

private slots:
    void SlotApplySure();
    void SlotApplyCancel();
};

#endif // APPLYFRIEND_H
