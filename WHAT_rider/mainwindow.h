#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>         // 메인 윈도우 기반
#include <QTcpSocket>          // TCP 소켓 사용
#include <QJsonDocument>       // JSON 처리
#include <QJsonObject>
#include <QPushButton>
#include <QtSql/qsqldatabase.h>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QJsonArray>
#include <QJsonValue>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    struct rider {
        QString phonenumber;
        QString pw;
        QString city;
        QString vehicle;
        QString name;
        QDate birth;
        QString account_number;
    };

    struct order_Info {
        QString order_id;
        QString uid;
        QString brand_uid;
        QString place_uid;
        QString total_price;
        QString order_time;
        QString address;
        QString address_detail;
        QString order_msg;
        QString rider_msg;
        QString status_togo;
        QString status_disposible;
        QString status_order;
    };

    void setDB();       //db접속설정    진영
    void delivery_list();                 //배달 가능 리스트 0616진영
    void back_main();                       //메인화면으로 돌아가기 진영
    void main_info();                       //메인화면 라이더 정보

    ~MainWindow();

private slots:
    //////////////서버////////////
    void on_login_button_clicked();       // 로그인 버튼 클릭 시
    void off_logut_button_cliked();       // 로그아웃 버튼 클릭 시
    void on_connected();                  // 서버 연결 성공 시
    void on_ready_read();                 // 데이터 수신 시
    void on_error(QAbstractSocket::SocketError err); // 에러 발생 시
    void login_handling();


private:
    Ui::MainWindow *ui;                   // UI 객체
    QTcpSocket *socket_;                  // TCP 소켓 객체

    rider rider_info;
    std::function<void(const QJsonObject&)> responseHandler_;

    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");  //진영 추가
    QByteArray recv_buffer;
};

#endif // MAINWINDOW_H
