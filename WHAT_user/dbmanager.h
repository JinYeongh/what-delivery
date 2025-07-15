#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include "struct_and_enum.h"  //============[0620]========

class DBManager : public QObject {
    Q_OBJECT

public:
    explicit DBManager(const QString& host,
                       const QString& dbName,
                       const QString& user,
                       const QString& password,
                       int port = 3306,
                       QObject *parent = nullptr);

    ~DBManager();

    bool isConnected() const;
    void db_send_order(const ORDER& order, const QList<ORDER_DETAIL>& details);         //=====[0620]=====


private:
    QSqlDatabase db_;
    bool connected_ = false;
    //---------기능 -------------
    QString check_adr(const int uid); // 기본주소 (현재배달주소) 찾는 쿼리
};

#endif // DBMANAGER_H
