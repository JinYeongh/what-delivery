#include "orderlist_widget.h"
#include "ui_orderlist_widget.h"
#include "mainwindow.h"
orderlist_widget::orderlist_widget(const ORDER &order,
                                    const QList<ORDER_DETAIL> &details,
                                   QWidget *parent)
    : QWidget(parent),
    ui(new Ui::orderlist_widget)
{
    ui->setupUi(this);

    // 1) 메뉴 & 옵션 목록
    QStringList menuLines;
    for (const auto &od : details) {
        QString line = QString("%1 x%2").arg(od.MENU_NAME).arg(od.MENU_CNT);
        if (!od.OPT_NAME_ALL.isEmpty())
            line += QString(" (%1)").arg(od.OPT_NAME_ALL);
        menuLines << line;
    }
    ui->label_selected_menus->setText(menuLines.join("\n"));

    // 2) 총 가격
    ui->label_totall_price->setText(QString::number(order.TOTAL_PRICE));

    // 3) 주소
    ui->label_address->setText(order.ADDRESS + " " + order.ADDRESS_DETAIL);

    // 4) 가게 요청사항
    ui->label_request->setText(order.ORDER_MSG.isEmpty() ? "-" : order.ORDER_MSG);

    // 5) 라이더 요청사항
    ui->label_rider_request->setText(order.RIDER_MSG.isEmpty() ? "-" : order.RIDER_MSG);

    // (필요시 수락/거절 버튼 시그널 연결도 여기서)
}

orderlist_widget::~orderlist_widget()
{
    delete ui;
}
