#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QJsonArray>
#include <QJsonDocument>
#include <QVBoxLayout>
#include <QDate>
#include <QTime>
#include <QPushButton>
#include <QHeaderView>

#include "orderlist_widget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , socket_(new QTcpSocket(this))
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);
    // --- connect 버튼 클릭 to 페이지 전환 슬롯 ---
    // page_login (index 0)
    // connect(ui->pushButton_login,     &QPushButton::clicked, this, &MainWindow::go_to_page_main_menu);
    connect(ui->pushButton_find_id,   &QPushButton::clicked, this, &MainWindow::go_to_page_find_id);
    connect(ui->pushButton_find_pw,   &QPushButton::clicked, this, &MainWindow::go_to_page_find_pw);
    connect(ui->pushButton_signin,    &QPushButton::clicked, this, &MainWindow::go_to_page_sign_in);

    // page_find_id (index 1)
    connect(ui->pushButton_back1,     &QPushButton::clicked, this, &MainWindow::go_to_page_login);
    // ※ pushButton_findid 는 아이디 찾기 로직용

    // page_find_pw (index 2)
    connect(ui->pushButton_back2,     &QPushButton::clicked, this, &MainWindow::go_to_page_find_id);
    // ※ pushButton_find 은 비밀번호 찾기 로직용

    // page_sign_in (index 3)
    connect(ui->pushButton_back3,     &QPushButton::clicked, this, &MainWindow::go_to_page_login);
    // ※ pushButton_add_file, pushButton_check_id, pushButton_real_signin 은 각각 로직용

    // page_main_menu (index 4)
    connect(ui->pushButton_check_orderlist, &QPushButton::clicked, this, &MainWindow::go_to_page_order);
    connect(ui->pushButton_logout_2,        &QPushButton::clicked, this, &MainWindow::go_to_page_login);
    // ※ pushButton_fix_save 는 저장 로직용

    // page_order (index 5)
    connect(ui->pushButton_back,        &QPushButton::clicked, this, &MainWindow::go_to_page_main_menu);
    connect(ui->pushButton_logout,      &QPushButton::clicked, this, &MainWindow::go_to_page_login);

    // 서버 연결
    on_login_button_clicked();
    connect(socket_, &QTcpSocket::connected, this, &MainWindow::on_connected);
    // connect(socket_, &QTcpSocket::readyRead, this, &MainWindow::on_ready_read);
    connect(socket_, &QTcpSocket::readyRead,
            this,    &MainWindow::onSocketReadyRead);

    connect(socket_, &QAbstractSocket::errorOccurred, this, &MainWindow::on_error);

    // 버튼 연결
    connect(ui->pushButton_login, &QPushButton::clicked, this, &MainWindow::sendLoginRequest);

    connect(ui->pushButton_check_orderlist, &QPushButton::clicked,
            this, &MainWindow::on_check_orderlist_clicked);


}

void MainWindow::go_to_page_login()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::go_to_page_find_id()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::go_to_page_find_pw()
{
    ui->stackedWidget->setCurrentIndex(2);
}

void MainWindow::go_to_page_sign_in()
{
    ui->stackedWidget->setCurrentIndex(3);
}

void MainWindow::go_to_page_main_menu()
{
    ui->stackedWidget->setCurrentIndex(4);
}

void MainWindow::go_to_page_order()
{
    ui->stackedWidget->setCurrentIndex(5);
    // populateOrderList();
}


void MainWindow::on_check_orderlist_clicked()
{
    ui->stackedWidget->setCurrentIndex(5);

    QJsonObject req;
    req["action"]    = "110_0";
    req["place_uid"] = owner_info.PLACE_UID;
    QByteArray ba = QJsonDocument(req).toJson(QJsonDocument::Compact);
    qDebug() << "▶ 주문 조회 요청 보냄:" << ba;
    socket_->write(ba);
}

void MainWindow::on_login_button_clicked()
{
    if (socket_->state() == QAbstractSocket::UnconnectedState) {
        socket_->connectToHost("10.10.20.122", SERVER_PORT);
    }
}

void MainWindow::on_connected()
{
    socket_->write(QString("owner").toUtf8());
    qDebug() << "owner 타입 전송 완료";
}
void MainWindow::sendLoginRequest()
{
    QJsonObject loginJson;
    loginJson["action"] = "100_0";
    // loginJson["phonenumber"] = ui->lineEdit_id->text();
    // loginJson["type"] = ui->lineEdit_pw->text();
    loginJson["phonenumber"] = "01012345678";
    loginJson["type"] = "owner";
    socket_->write(QJsonDocument(loginJson).toJson(QJsonDocument::Compact));
}



void MainWindow::handleOrderResponse(const QJsonObject &resp)
{
    qDebug() << "사장 주문 목록 수신:" << resp;

    // 1) 기존 스크롤 위젯 있으면 제거
    if (QWidget *old = ui->scrollArea_order->takeWidget()) {
        old->deleteLater();
    }

    // 2) 새로운 컨테이너와 레이아웃 생성
    QWidget *container = new QWidget;
    // 스크롤 영역이 흘러내리지 않도록 사이즈 정책 설정
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    QVBoxLayout *vbox = new QVBoxLayout(container);
    vbox->setAlignment(Qt::AlignTop);  // 위에서부터 차곡차곡

    // 3) JSON 파싱하여 orderlist_widget 생성
    QJsonArray orders = resp["orders"].toArray();
    for (const QJsonValue &val : orders) {
        QJsonObject o = val.toObject();

        // ORDER 구조체에 담기
        ORDER ord;
        ord.ORDER_ID        = o["order_id"].toString();
        ord.UID             = o["uid"].toInt();
        ord.BRAND_UID       = o["brand_uid"].toInt();
        ord.PLACE_UID       = o["place_uid"].toInt();
        ord.TOTAL_PRICE     = o["total_price"].toInt();
        ord.ORDER_TIME      = o["order_time"].toString();
        ord.ADDRESS         = o["address"].toString();
        ord.ADDRESS_DETAIL  = o["address_detail"].toString();
        ord.ORDER_MSG       = o["order_msg"].toString();
        ord.RIDER_MSG       = o["rider_msg"].toString();
        ord.STATUS_TOGO     = o["status_togo"].toInt();
        ord.STATUS_DISPOSIBLE = o["status_disposible"].toBool();
        ord.STATUS_ORDER    = o["status_order"].toInt();

        // MainWindow::ORDER_DETAIL 리스트에 담기
        QList<ORDER_DETAIL> detailsList;
        for (const QJsonValue &dv : o["details"].toArray()) {
            QJsonObject d = dv.toObject();
            ORDER_DETAIL od;
            od.ORDER_ID      = ord.ORDER_ID;
            od.MENU_NAME     = d["menu_name"].toString();
            od.MENU_PRICE    = d["menu_price"].toInt();
            od.MENU_CNT      = d["menu_cnt"].toInt();
            od.OPT_NAME_ALL  = d["opt_name_all"].toString();
            od.OPT_PRICE_ALL = d["opt_price_all"].toInt();
            detailsList.append(od);
        }

        // orderlist_widget 생성·추가
        auto *w = new orderlist_widget(ord, detailsList, this);
        vbox->addWidget(w);
    }

    // 4) 컨테이너를 스크롤영역에 배치
    container->setLayout(vbox);
    ui->scrollArea_order->setWidget(container);
    ui->scrollArea_order->setWidgetResizable(true);
}

//------------------------------------------------------------------------------
void MainWindow::order_handling(const QJsonObject &resp)
{
    qDebug() << "사장 주문 목록 수신:" << resp;
    QWidget *container = new QWidget;
    QVBoxLayout *vbox = new QVBoxLayout(container);

    auto orders = resp["orders"].toArray();
    for (const auto &val : orders) {
        auto o = val.toObject();
        ORDER ord;
        ord.ORDER_ID       = o["order_id"].toString();
        ord.UID            = o["uid"].toInt();
        ord.BRAND_UID      = o["brand_uid"].toInt();
        ord.PLACE_UID      = o["place_uid"].toInt();
        ord.TOTAL_PRICE    = o["total_price"].toInt();
        ord.ORDER_TIME     = o["order_time"].toString();
        ord.ADDRESS        = o["address"].toString();
        ord.ADDRESS_DETAIL = o["address_detail"].toString();
        ord.ORDER_MSG      = o["order_msg"].toString();
        ord.RIDER_MSG      = o["rider_msg"].toString();
        ord.STATUS_TOGO    = o["status_togo"].toInt();
        ord.STATUS_DISPOSIBLE = o["status_disposible"].toBool();
        ord.STATUS_ORDER   = o["status_order"].toInt();

        QList<ORDER_DETAIL> detailsList;
        for (const auto &dv : o["details"].toArray()) {
            auto d = dv.toObject();
            ORDER_DETAIL od;
            od.ORDER_ID      = ord.ORDER_ID;
            od.MENU_NAME     = d["menu_name"].toString();
            od.MENU_PRICE    = d["menu_price"].toInt();
            od.MENU_CNT      = d["menu_cnt"].toInt();
            od.OPT_NAME_ALL  = d["opt_name_all"].toString();
            od.OPT_PRICE_ALL = d["opt_price_all"].toInt();
            detailsList.append(od);
        }

        auto *w = new orderlist_widget(ord, detailsList, this);
        vbox->addWidget(w);
    }

    container->setLayout(vbox);
    ui->scrollArea_order->setWidget(container);
}

void MainWindow::handleLoginResponse(const QJsonObject &resp)
{
    // 성공(100_1) / 실패(100_2) 분기
    if (resp["action"].toString() == "100_1") {
        qDebug() << "로그인 성공";
        owner_info.PHONENUMBER             = resp["PHONENUMBER"].toString();
        owner_info.type                    = resp["type"].toString();
        owner_info.BRAND_UID               = resp["BRAND_UID"].toString();
        owner_info.PLACE_UID               = resp["PLACE_UID"].toString();
        owner_info.ADDRESS                 = resp["ADDRESS"].toString();
        owner_info.ADDRESS_DETAIL          = resp["ADDRESS_DETAIL"].toString();
        owner_info.OPEN_DATE               = QDate::fromString(resp["OPEN_DATE"].toString(), "yyyy-MM-dd");
        owner_info.DELIVERY_TIM            = resp["DELIVERY_TIM"].toString();
        owner_info.OPEN_TIME               = QTime::fromString(resp["OPEN_TIME"].toString(), "HH:mm");
        owner_info.CLOSE_TIME              = QTime::fromString(resp["CLOSE_TIME"].toString(), "HH:mm");
        owner_info.ORIGIN_INFO             = resp["ORIGIN_INFO"].toString();
        owner_info.MIN_ORDER_PRICE         = resp["MIN_ORDER_PRICE"].toString().toInt();
        owner_info.DELIVERY_FEE            = resp["DELIVERY_FEE"].toString().toInt();
        owner_info.FREE_DELIVERY_STANDARD  = resp["FREE_DELIVERY_STANDARD"].toString().toInt();
        owner_info.IMAGE_PASS              = resp["IMAGE_PASS"].toString();
        owner_info.IMAGE_MAIN              = resp["IMAGE_MAIN"].toString();
        // 스택 위젯 페이지 전환
        ui->stackedWidget->setCurrentIndex(4);
        populateOwnerTable();
    }
    else {
        QMessageBox::warning(this, "로그인 실패", resp["message"].toString());
    }
}

// void MainWindow::on_ready_read()
// {
//     auto resp   = QJsonDocument::fromJson(socket_->readAll()).object();
//     qDebug() << "▶ 전체 JSON 응답:" << resp;
//     auto header = resp["action"].toString();
//     qDebug() << "▶ action 필드:" << header;

//     if (header == "100_1" || header == "100_2") {
//         handleLoginResponse(resp);
//     }
//     else if (header == "110_1") {
//         qDebug() << "▶ 주문 처리 분기 진입!";  // 확인용
//         order_handling(resp);
//     }
// }

void MainWindow::onSocketReadyRead()
{
    recv_buffer += QString::fromUtf8(socket_->readAll());
    const QString delimiter = "<<END>>";
    int pos;
    while ((pos = recv_buffer.indexOf(delimiter)) != -1) {
        QString jsonText = recv_buffer.left(pos).trimmed();
        recv_buffer.remove(0, pos + delimiter.length());

        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(jsonText.toUtf8(), &err);
        if (err.error != QJsonParseError::NoError) {
            qWarning() << "JSON 파싱 실패:" << err.errorString();
            continue;
        }
        QJsonObject resp = doc.object();

        // 1) 로그인 응답
        if (resp.value("action").toString() == "100_1") {
            handleLoginResponse(resp);
            return;
        }
        // 2) 주문 목록 응답
        if (resp.contains("orders")) {
            handleOrderResponse(resp);
            return;
        }
        // 추가 액션이 있으면 여기에 더 처리…
    }
}

void MainWindow::populateOwnerTable()
{
    // 표시할 필드와 실제 값을 쌍으로 준비
    struct Field { QString label, value; };
    QVector<Field> fields = {
                             { "주소",                    owner_info.ADDRESS + " " + owner_info.ADDRESS_DETAIL },
                             { "입점일",                  owner_info.OPEN_DATE.toString("yyyy-MM-dd") },
                             { "전화번호",                owner_info.PHONENUMBER },
                             { "평균배달시간",            owner_info.DELIVERY_TIM },
                             { "오픈시간",                owner_info.OPEN_TIME.toString("HH:mm") },
                             { "마감시간",                owner_info.CLOSE_TIME.toString("HH:mm") },
                             { "원산지정보",              owner_info.ORIGIN_INFO },
                             { "최소주문금액",            QString::number(owner_info.MIN_ORDER_PRICE) },
                             { "배달비",                  QString::number(owner_info.DELIVERY_FEE) },
                             { "무료배달 기준금액",       QString::number(owner_info.FREE_DELIVERY_STANDARD) },
                             { "아이콘 경로",             owner_info.IMAGE_PASS },
                             { "메인 이미지 경로",        owner_info.IMAGE_MAIN },
                             };

    auto *tw = ui->tableWidget;
    tw->clear();
    tw->setRowCount(fields.size());
    tw->setColumnCount(2);
    tw->setHorizontalHeaderLabels({ "항목", "값" });
    tw->verticalHeader()->setVisible(false);

    tw->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    tw->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);

    for (int i = 0; i < fields.size(); ++i) {
        tw->setItem(i, 0, new QTableWidgetItem(fields[i].label));
        tw->setItem(i, 1, new QTableWidgetItem(fields[i].value));
    }
}


void MainWindow::on_error(QAbstractSocket::SocketError err)
{
    if (err == QAbstractSocket::RemoteHostClosedError)
        return;
    QMessageBox::critical(this, "소켓 에러", socket_->errorString());
}

MainWindow::~MainWindow()
{
    delete ui;
}
