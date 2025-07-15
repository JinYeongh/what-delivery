#include "sel_menu_info.h"
#include "ui_sel_menu_info.h"
#include "struct_and_enum.h"
#include <QCheckBox>
#include <QHBoxLayout>
#include <QTableWidgetItem>
#include <QRandomGenerator>

// 옵션 추출 함수
QVector<Option> extractOptions(const menu& m) {
    QVector<Option> options;

    const QString* name_ptrs[] = {
        &m.option1, &m.option2, &m.option3, &m.option4, &m.option5,
        &m.option6, &m.option7, &m.option8, &m.option9, &m.option10
    };

    const QString* price_ptrs[] = {
        &m.option1_price, &m.option2_price, &m.option3_price, &m.option4_price, &m.option5_price,
        &m.option6_price, &m.option7_price, &m.option8_price, &m.option9_price, &m.option10_price
    };

    for (int i = 0; i < 10; ++i) {
        QString name = name_ptrs[i]->trimmed();
        QString price = price_ptrs[i]->trimmed();

        if (!name.isEmpty() && name.toUpper() != "NULL") {
            options.append({ name, price });
        }
    }
    return options;
}

sel_menu_info::sel_menu_info(QWidget *parent, const menu& m, const store_info& s)
    : QWidget(parent), ui(new Ui::sel_menu_info), current_menu(m)
{
    ui->setupUi(this);

    // 최소 주문 금액 표시
    int minOrder = s.min_order_price.toInt();
    QString formatted = QLocale(QLocale::Korean).toString(minOrder);  // "11,000"
    ui->delivery_start->setText(formatted + "원부터 배달 가능해요");


    // 뒤로가기 버튼
    QPushButton* backBtn = new QPushButton("<", ui->menu_img);
    backBtn->setFixedSize(30, 30);
    backBtn->move(10, 10);
    backBtn->setStyleSheet("QPushButton { background-color: rgba(0,0,0,0.6); color:white; font-size:16px; border:none; font-weight:bold; border-radius:15px; }");
    connect(backBtn, &QPushButton::clicked, this, [=]() { emit backButtonClicked(); });

    // 수량 버튼 연결
    connect(ui->cnt_f_Btn, &QPushButton::clicked, this, &sel_menu_info::plus_clicked);
    connect(ui->cnt_m_Btn, &QPushButton::clicked, this, &sel_menu_info::minus_clicked);
    connect(ui->go_cart_Btn, &QPushButton::clicked, this, &sel_menu_info::cart_clicked);

    basePrice = m.price.toInt();    //이 변수에 가격 저장
    ui->cnt_num->setText("1");

    // 이미지 세팅
    QPixmap pix(m.img);
    ui->menu_img->setPixmap(pix);
    ui->menu_img->setScaledContents(true);

    // 메뉴 정보 표시
    ui->menu_name->setText(m.name);
    int price = m.price.toInt();
    QString formattedPrice = QLocale(QLocale::Korean).toString(price);
    ui->menu_price->setText(formattedPrice + "원");

    ui->menu_info->setText(m.menu_info);

    // 랜덤 리뷰 수 설정
    int reviewCount = QRandomGenerator::global()->bounded(1, 201);  // 1 ~ 200 사이 숫자
    ui->reviewnum_label->setText(QString::number(reviewCount));

    // 옵션 설정
    options = extractOptions(m);  // 멤버 변수에 저장됨

    // 옵션 테이블 위젯 구성
    ui->option_widget->setRowCount(options.size());
    ui->option_widget->setColumnCount(3);
    ui->option_widget->horizontalHeader()->hide();         //헤더 숨기기
    ui->option_widget->verticalHeader()->setVisible(false); //세로 헤더 숨기기
    ui->option_widget->setStyleSheet("QTableWidget { border: none; }"); //셀 테두리 없애기
    ui->option_widget->setShowGrid(false);                 //셀 가운데 구분선 없애기
    ui->option_widget->setFocusPolicy(Qt::NoFocus);        // 포커스 제거 0619 진
    ui->option_widget->setSelectionMode(QAbstractItemView::NoSelection);  // 선택 자체 제거 0619 진
    ui->option_widget->setEditTriggers(QAbstractItemView::NoEditTriggers); // 수정 비활성화
    ui->option_widget->setColumnWidth(0, 30);              // 0번째 열(체크박스) 고정
    ui->option_widget->horizontalHeader()->setStretchLastSection(true);   // 마지막 열 늘리기
    // 스크롤 바 숨기기
    ui->option_widget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->option_widget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // 테이블 뒤 클릭 방지
    ui->option_widget->setAttribute(Qt::WA_TransparentForMouseEvents, false);
    ui->option_widget->setEnabled(true);  // 명시적으로 활성화

    for (int i = 0; i < options.size(); ++i) {
        const Option& opt = options[i];

        QCheckBox* checkBox = new QCheckBox(this);
        connect(checkBox, &QCheckBox::stateChanged, this, &sel_menu_info::option_Check);
        checkBoxes.append(checkBox);

        QWidget* checkWidget = new QWidget(this);
        QHBoxLayout* layout = new QHBoxLayout(checkWidget);
        layout->addWidget(checkBox);
        layout->setAlignment(Qt::AlignCenter);
        layout->setContentsMargins(0, 0, 0, 0);
        checkWidget->setLayout(layout);
        ui->option_widget->setCellWidget(i, 0, checkWidget);

        // 옵션명
        QTableWidgetItem* nameItem = new QTableWidgetItem(opt.name);
        ui->option_widget->setItem(i, 1, nameItem);

        // 가격 포맷
        bool ok;
        int price = opt.price.toInt(&ok);
        QString priceStr;
        if (ok && price > 0) {
            priceStr = "+" + QLocale(QLocale::Korean).toString(price) + "원";  //+1,000원
        }

        QTableWidgetItem* priceItem = new QTableWidgetItem(priceStr);
        priceItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        ui->option_widget->setItem(i, 2, priceItem);
    }

    // updateTotalPrice();  // 초기 총 가격 표시
}

// 옵션 체크박스 변경 시 호출되는 함수
void sel_menu_info::option_Check() {
    updateTotalPrice();  // 총 가격 다시 계산해서 버튼 텍스트 갱신
}


// 수량 + 버튼 클릭
void sel_menu_info::plus_clicked() {
    quantity++;
    ui->cnt_num->setText(QString::number(quantity));
    updateTotalPrice();  // 총 가격만 업데이트
}

// 수량 - 버튼 클릭
void sel_menu_info::minus_clicked() {
    if (quantity > 1) {
        quantity--;
        ui->cnt_num->setText(QString::number(quantity));
        updateTotalPrice();  // 총 가격만 업데이트
    }
}

// '담기' 버튼 클릭
void sel_menu_info::cart_clicked() {
    // 주문 상세 정보 구조체 생성
    ORDER_DETAIL detail;
    detail.ORDER_ID = "";
    detail.MENU_NAME = ui->menu_name->text();
    detail.MENU_PRICE = basePrice;
    detail.MENU_CNT = quantity;
    detail.MENU_IMG_PATH = current_menu.img;  //0619 추가 진

    // 선택된 옵션들 정리
    QStringList optNames;
    int optTotal = 0;

    for (int i = 0; i < checkBoxes.size(); ++i) {
        if (checkBoxes[i]->isChecked()) {
            optNames << options[i].name;
            optTotal += options[i].price.toInt();
        }
    }

    detail.OPT_NAME_ALL = optNames.join(", ");
    detail.OPT_PRICE_ALL = optTotal;

    qDebug() << "담기 버튼 클릭됨. 주문 전송:";
    qDebug() << "메뉴명:" << detail.MENU_NAME;
    qDebug() << "옵션:" << detail.OPT_NAME_ALL;
    qDebug() << "총 수량:" << detail.MENU_CNT;

    // 총 누적 금액 계산
    int addedAmount = (basePrice + optTotal) * quantity;
    totalCartAmount += addedAmount;
    qDebug() << "이번에 담긴 금액:" << addedAmount << "원";
    qDebug() << "현재까지 총 누적 금액:" << totalCartAmount << "원";

    // 주문 완료 시그널 및 화면 전환
    emit orderCompleted(detail);  // MainWindow로 주문 정보 전달
    emit backButtonClicked();     // 스택을 페이지4로 보내기
}

// 총 가격
void sel_menu_info::updateTotalPrice() {
    int optPrice = 0;
    for (int i = 0; i < checkBoxes.size(); ++i) {
        if (checkBoxes[i]->isChecked())
            optPrice += options[i].price.toInt();
    }

    int total = (basePrice + optPrice) * quantity;
    QString formatted = QLocale(QLocale::Korean).toString(total);  // "3,800"
    ui->go_cart_Btn->setText(QString("%1원 담기").arg(formatted));
}




sel_menu_info::sel_menu_info(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::sel_menu_info)
{
    ui->setupUi(this);
}

sel_menu_info::~sel_menu_info()
{
    delete ui;
}
