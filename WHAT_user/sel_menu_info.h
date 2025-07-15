#ifndef SEL_MENU_INFO_H
#define SEL_MENU_INFO_H

#include <QWidget>
#include <QCheckBox>
#include <QVector>
#include "struct_and_enum.h"

struct Option {
    QString name;
    QString price;
};

namespace Ui {
class sel_menu_info;
}

class sel_menu_info : public QWidget
{
    Q_OBJECT

public:
    explicit sel_menu_info(QWidget *parent = nullptr);
    explicit sel_menu_info(QWidget *parent, const menu& m);
    explicit sel_menu_info(QWidget *parent, const menu& m, const store_info& s);
    void updateTotalPrice();     // 가격 계산 추가함요
    ~sel_menu_info();


signals:
    void backButtonClicked();
    void orderCompleted(const ORDER_DETAIL& order);  // 주문 완료 시 MainWindow로 전달


private slots:
    void option_Check();      // 옵션 체크박스 변경
    void plus_clicked();        // 수량 +
    void minus_clicked();       // 수량 -
    void cart_clicked();   // 담기 클릭

private:
    Ui::sel_menu_info *ui;
    QVector<QCheckBox*> checkBoxes;
    QVector<Option> options;
    int basePrice = 0;
    int quantity = 1;
    menu current_menu;  // 현재 선택한 메뉴 저장
    int totalCartAmount = 0;  // 지금까지 담긴 총 금액
};



#endif // SEL_MENU_INFO_H
