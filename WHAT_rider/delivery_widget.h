#ifndef DELIVERY_WIDGET_H
#define DELIVERY_WIDGET_H

#include <QWidget>

namespace Ui {
class delivery_widget;
}

class delivery_widget : public QWidget
{
    Q_OBJECT

public:
    explicit delivery_widget(QWidget *parent = nullptr, QString shop_adr = "", QString user_adr = "", QString rider_msg = "");
    ~delivery_widget();

private:
    Ui::delivery_widget *ui;
};

#endif // DELIVERY_WIDGET_H
