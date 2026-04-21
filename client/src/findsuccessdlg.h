#ifndef FINDSUCCESSDLG_H
#define FINDSUCCESSDLG_H

#include <QDialog>
#include "userdata.h"
#include <memory>

namespace Ui {
class FindSuccessDlg;
}

class FindSuccessDlg : public QDialog
{
    Q_OBJECT

public:
    explicit FindSuccessDlg(QWidget *parent = nullptr);
    ~FindSuccessDlg();
    void SetSearchInfo(std::shared_ptr<SearchInfo> si);

private:
    Ui::FindSuccessDlg *ui;
    std::shared_ptr<SearchInfo> _si;
    QWidget* _parent;

private slots:
    void on_add_friend_btn_clicked();
};

#endif // FINDSUCCESSDLG_H
