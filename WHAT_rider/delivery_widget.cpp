#include "delivery_widget.h"
#include "ui_delivery_widget.h"

delivery_widget::delivery_widget(QWidget *parent, QString shop_adr, QString user_adr, QString rider_msg)
    : QWidget(parent)
    , ui(new Ui::delivery_widget)
{
    ui->setupUi(this);
    ui->shop_info->setText(shop_adr);
    ui->user_address->setText(user_adr);
    ui->rider_msg->setText(rider_msg);
}

delivery_widget::~delivery_widget()
{
    delete ui;
}
