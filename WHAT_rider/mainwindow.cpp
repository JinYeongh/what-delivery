#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "delivery_widget.h"

#include <QMessageBox>
#include <QLayout>

#define SERVER_PORT 12345

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , socket_(new QTcpSocket(this))
{
    ui->setupUi(this);

    // 버튼 및 소켓 시그널 연결
    on_login_button_clicked();
    ui->stackedWidget->setCurrentIndex(0);
    connect(ui->pushButton_login, &QPushButton::clicked, this, &MainWindow::login_handling);
    connect(socket_, &QTcpSocket::connected, this, &MainWindow::on_connected);
    connect(socket_, &QTcpSocket::readyRead, this, &MainWindow::on_ready_read);
    connect(socket_, &QAbstractSocket::errorOccurred,this, &MainWindow::on_error);
    connect(ui->del_start_Btn, &QPushButton::clicked, this, &MainWindow::delivery_list);
    connect(ui->backmain_Btn, &QPushButton::clicked, this, &MainWindow::back_main);
    connect(ui->logout_Btn, &QPushButton::clicked, this, &MainWindow::off_logut_button_cliked);
}

//서버
void MainWindow::on_login_button_clicked() {
    // 서버에 연결 시도
    if (socket_->state() == QAbstractSocket::UnconnectedState) {
        socket_->connectToHost("10.10.20.99", SERVER_PORT);
    }
}

void MainWindow::setDB(){       // DB 접속 설정 진영 0616
    this->db.setHostName("10.10.20.99");
    this->db.setPort(3306);       // 이것만 정수
    this->db.setUserName("JIN");
    this->db.setPassword("1234");
    this->db.setDatabaseName("WHAT");
}

void MainWindow::off_logut_button_cliked(){ //로그아웃 클릭
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::back_main() {  //주문목록에서 메인화면으로 돌아가기
    ui->lineEdit_id->clear();
    ui->lineEdit_pw->clear();
    ui->stackedWidget->setCurrentIndex(4);

}

void MainWindow::on_connected() {
    QByteArray data = QString("rider").toUtf8();
    socket_->write(data);
    qDebug() << "rider 타입 전송 완료";
}

void MainWindow::login_handling() {  //서현 수정
    // 로그인 JSON 생성 및 전송

    QJsonObject loginJson;
    loginJson["action"] = "1000_0";
    // loginJson["phonenumber"] = ui->lineEdit_id->text();
    // loginJson["pw"] = ui->lineEdit_pw->text();
    loginJson["phonenumber"] = "01079794242";
    loginJson["pw"] = "1234";

    responseHandler_ = [this](const QJsonObject& json) {
        if (json["action"] == "1000_1") {
            qDebug() << "로그인 성공";

            this->rider_info.phonenumber = json["phonenumber"].toString();
            this->rider_info.pw = json["pw"].toString();  // 입력값 그대로
            this->rider_info.city = json["city"].toString();
            this->rider_info.vehicle = json["vehicle"].toString();
            this->rider_info.name = json["name"].toString();
            this->rider_info.birth = QDate::fromString(json["birth"].toString(), "yyyy-MM-dd");
            this->rider_info.account_number = json["account_number"].toString();

            QMessageBox::information(this, "로그인 성공", json["message"].toString());
            ui->stackedWidget->setCurrentIndex(4);
            main_info();
        } else {
            QMessageBox::warning(this, "로그인 실패", json["message"].toString());
        }
    };


    socket_->write(QJsonDocument(loginJson).toJson(QJsonDocument::Compact));
}
void MainWindow::main_info()
{
    struct Field { QString label, value; };
    QVector<Field> fields = {
                             { "이름",          rider_info.name },
                             { "생년월일",      rider_info.birth.toString("yyyy-MM-dd") },
                             { "도시",          rider_info.city },
                             { "운송수단",      rider_info.vehicle },
                             { "계좌번호",      rider_info.account_number },
                             { "전화번호",      rider_info.phonenumber },
                             };

    auto* tw = ui->rider_info;
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

void MainWindow::delivery_list() {
    ui->stackedWidget->setCurrentIndex(5);

    QJsonObject request;
    request["action"] = "1001_0";

    responseHandler_ = [this](const QJsonObject& json) {
        if (json["action"] == "1001_1") {
            qDebug() << "배달목록 조회 성공";

            // 기존 UI 위젯 제거 (초기화)
            QLayoutItem *child;
            while ((child = ui->deliveryListLayout->takeAt(0)) != nullptr) {
                if (child->widget()) {
                    child->widget()->deleteLater();
                }
                delete child;
            }

            // 주문 배열 반복
            QJsonArray orderArray = json["orders"].toArray();
            for (const QJsonValue& value : orderArray) {
                QJsonObject order = value.toObject();

                QString shop_adr = order["owner_full_address"].toString();
                QString user_adr = order["user_full_address"].toString();
                QString rider_msg = order["RIDER_MSG"].toString();

                qDebug() << "shop_adr:" << shop_adr << "user_adr:" << user_adr << "rider_msg:" << rider_msg;

                delivery_widget* widget = new delivery_widget(this, shop_adr, user_adr, rider_msg);
                ui->deliveryListLayout->addWidget(widget);
            }

        } else {
            QMessageBox::warning(this, "조회 실패", json["message"].toString());
        }
    };

    socket_->write(QJsonDocument(request).toJson(QJsonDocument::Compact));
}



void MainWindow::on_ready_read() {
    recv_buffer += socket_->readAll();

    int endIdx;
    while ((endIdx = recv_buffer.indexOf("<<END>>")) != -1) {
        QByteArray completeMsg = recv_buffer.left(endIdx);
        recv_buffer.remove(0, endIdx + strlen("<<END>>"));

        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(completeMsg, &err);

        if (err.error != QJsonParseError::NoError) {
            qDebug() << "[JSON 파싱 오류]" << err.errorString();
            continue;
        }

        QJsonObject json = doc.object();
        if (responseHandler_) {
            responseHandler_(json);
            responseHandler_ = nullptr;
        }
    }
}

void MainWindow::on_error(QAbstractSocket::SocketError err) {  //서현 수정
    if (err == QAbstractSocket::RemoteHostClosedError) {
        qDebug() << "서버가 정상적으로 연결을 종료했습니다.";
        return;
    }
    QMessageBox::critical(this, "소켓 에러", socket_->errorString());
}


MainWindow::~MainWindow()
{
    delete ui;
}
