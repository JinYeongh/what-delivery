#ifndef ADDRESSBTN_H
#define ADDRESSBTN_H

#include <QWidget>

namespace Ui {
class addressBtn;
}

class addressBtn : public QWidget
{
    Q_OBJECT

public:
    explicit addressBtn(QWidget *parent = nullptr, QString address = "", QString address_detail = "");
    ~addressBtn();

private:
    Ui::addressBtn *ui;
    QString member_address_detail;

signals:
    void Address_clicked(QString address);   // 주소 클릭
    void Address_delete_clicked(QString address, QWidget* sender);          // 수정 클릭

};

#endif // ADDRESSBTN_H
