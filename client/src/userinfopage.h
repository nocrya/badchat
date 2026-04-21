#ifndef USERINFOPAGE_H
#define USERINFOPAGE_H

#include <QWidget>

namespace Ui {
class UserInfoPage;
}

class UserInfoPage : public QWidget
{
    Q_OBJECT

public:
    explicit UserInfoPage(QWidget *parent = nullptr);
    ~UserInfoPage();

private slots:
    void on_up_btn_clicked();
    QString generateUniqueIconName();

    void on_submit_btn_clicked();

private:
    Ui::UserInfoPage *ui;
    QPixmap _scaledPixmap;
};

#endif // USERINFOPAGE_H
