#include "menu_head.h"
#include "ui_menu_head.h"

#include <QLocale>
#include <QMouseEvent>

menu_head::menu_head(QWidget *parent, QString img, QString name, QString price)
    : QWidget(parent)
    , ui(new Ui::menu_head)
    , menu_name(name)

{
    ui->setupUi(this);

    QLocale locale(QLocale::Korean);
    int price_int = price.toInt();
    QString formattedPrice = locale.toString(price_int);

    ui->menu_head_img->setStyleSheet(QString("border-image: url(%1)").arg(img));
    ui->menu_head_name->setText(name);
    ui->menu_head_price->setText(QString("%1원").arg(formattedPrice));
}



menu_head::~menu_head()
{
    delete ui;
}

