#include "dbmanager.h"
#include <QObject>
DBManager::DBManager(const QString& host,
                     const QString& dbName,
                     const QString& user,
                     const QString& password,
                     int port,
                     QObject *parent)
    : QObject(parent)
{

    db_ = QSqlDatabase::addDatabase("QMYSQL");
    db_.setHostName(host);
    db_.setDatabaseName(dbName);
    db_.setUserName(user);
    db_.setPassword(password);
    db_.setPort(port);

    if (db_.open()) {
        connected_ = true;
        qDebug() << "[DB 연결 성공]";
    } else {
        qDebug() << "[DB 연결 실패]:" << db_.lastError().text();
    }
}

DBManager::~DBManager() {
    if (db_.isOpen()) db_.close();
}

bool DBManager::isConnected() const {
    return connected_;
}

QString DBManager::check_adr(const int uid) {
    if (!connected_) return QString();

    QSqlQuery query;
    query.prepare("SELECT ADDRESS_DETAIL FROM USER_ADDRESS WHERE UID = ? AND ADDRESS_BASIC = 1");
    query.addBindValue(uid);

    if (!query.exec()) {
        qDebug() << "[기본 주소 조회 실패]:" << query.lastError().text();
        return QString();  // 실패시 빈 문자열 반환
    }

    if (query.next()) {
        return query.value(0).toString();  // ADDRESS_DETAIL 반환
    } else {
        return QString();  // 기본 주소 없음
    }
}

void DBManager::db_send_order(const ORDER& order, const QList<ORDER_DETAIL>& details) {
    if (!connected_) {
        qDebug() << "[DB 연결 안됨]";
        return;
    }

    QSqlDatabase db = db_;  //이거 필요한가?
    QSqlQuery query(db);

    // ORDER_ 테이블에 INSERT
    query.prepare(R"(
        INSERT INTO ORDER_ (
            ORDER_ID, UID, BRAND_UID, PLACE_UID,
            TOTAL_PRICE, ORDER_TIME,
            ADDRESS, ADDRESS_DETAIL,
            ORDER_MSG, RIDER_MSG,
            STATUS_TOGO, STATUS_DISPOSIBLE, STATUS_ORDER
        )
        VALUES (?, ?, ?, ?, ?, NOW(), ?, ?, ?, ?, ?, ?, ?)
    )");

    query.addBindValue(order.ORDER_ID);
    query.addBindValue(order.UID);
    query.addBindValue(order.BRAND_UID);
    query.addBindValue(order.PLACE_UID);
    query.addBindValue(order.TOTAL_PRICE);
    query.addBindValue(order.ADDRESS);
    query.addBindValue(order.ADDRESS_DETAIL);
    query.addBindValue(order.ORDER_MSG);
    query.addBindValue(order.RIDER_MSG);
    query.addBindValue(order.STATUS_TOGO);
    query.addBindValue(order.STATUS_DISPOSIBLE);
    query.addBindValue(order.STATUS_ORDER);

    if (!query.exec()) {
        qDebug() << "[ORDERS INSERT 실패]" << query.lastError().text();
        return;
    }

    // ORDER_DETAIL 테이블에 여러 개 INSERT
    for (const ORDER_DETAIL& d : details) {
        QSqlQuery detail_query(db);
        detail_query.prepare(R"(
            INSERT INTO ORDER_DETAIL (
                ORDER_ID, MENU_NAME, MENU_PRICE, MENU_CNT,
                OPT_NAME_ALL, OPT_PRICE_ALL
            )
            VALUES (?, ?, ?, ?, ?, ?)
        )");

        detail_query.addBindValue(d.ORDER_ID);
        detail_query.addBindValue(d.MENU_NAME);
        detail_query.addBindValue(d.MENU_PRICE);
        detail_query.addBindValue(d.MENU_CNT);
        detail_query.addBindValue(d.OPT_NAME_ALL);
        detail_query.addBindValue(d.OPT_PRICE_ALL);

        if (!detail_query.exec()) {
            qDebug() << "[ORDER_DETAIL INSERT 실패]" << detail_query.lastError().text();
        }
    }

    qDebug() << "[주문 및 메뉴 INSERT 완료]";
}
