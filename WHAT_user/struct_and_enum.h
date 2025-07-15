#ifndef STRUCT_AND_ENUM_H
#define STRUCT_AND_ENUM_H

#include <QString>
#include <QList>

#define SERVER_PORT        12345
#define GO_LOGIN_MAIN      1
#define GO_HOME            2
#define GO_STORE_AND_MENU  3
#define GO_STORE_SELECT    4
#define GO_ADDRESS         5
#define GO_ORDER           6
#define GO_HEART           7
#define GO_MY              8
#define GO_MY_REVIEW       9
#define GO_CART           10
#define GO_CART2          11
#define GO_PAYING         12
#define GO_PAY_COMPLETE   13
#define GO_WRITE_REVIEW   14
#define GO_ORDER_DETAIL   15
#define GO_MENU_DETAIL    17


// 사용자 정보 구조체
struct user {
    int uid;
    QString id;
    QString Email_id;
    QString name;
    QString birth;
    QString phonenum;
    int pass;
    int point;
};

// 주소 정보 구조체
struct address {
    QString adr_name;
    QString adr;
    QString adr_detail;
    QString adr_type;
    int adr_default;
};
//가게 정보 구조체
struct store_info {
    QString brand_uid;
    QString place_uid;
    QString name;
    QString address;
    QString address_detail;
    QString open_date;
    QString phonenumber;
    QString delivery_time;
    QString open_time;
    QString close_time;
    QString origin_info;
    QString min_order_price;
    QString delivery_fee;
    QString free_delivery_standard;
    QString image_main;
    QString image_pass;
};

//메뉴 정보 구조체
struct menu{
    QString brand_uid;
    QString name;
    QString category;
    QString menu_info;
    QString img;
    QString price;
    QString option1;
    QString option1_price;
    QString option2;
    QString option2_price;
    QString option3;
    QString option3_price;
    QString option4;
    QString option4_price;
    QString option5;
    QString option5_price;
    QString option6;
    QString option6_price;
    QString option7;
    QString option7_price;
    QString option8;
    QString option8_price;
    QString option9;
    QString option9_price;
    QString option10;
    QString option10_price;
};

// 주문 정보 구조체
struct ORDER {
    QString ORDER_ID;
    int UID;
    int BRAND_UID;
    int PLACE_UID;
    int TOTAL_PRICE;
    QString ORDER_TIME;
    QString ADDRESS;         // DB 컬럼 ADDRESS 추가
    QString ADDRESS_DETAIL;  // DB 컬럼 ADDRESS_DETAIL 추가
    QString ORDER_MSG;
    QString RIDER_MSG;       // DB 컬럼 RIDER_MSG 추가
    int STATUS_TOGO;
    bool STATUS_DISPOSIBLE;
    int STATUS_ORDER;
};

// 주문 상세 항목 구조체
struct ORDER_DETAIL {
    QString ORDER_ID;
    QString MENU_NAME;
    int MENU_PRICE;
    int MENU_CNT;
    QString OPT_NAME_ALL;
    int OPT_PRICE_ALL;
    QString MENU_IMG_PATH;
};


// 카테고리 열거형
enum category {
    CATEGORY_ALL,
    CATEGORY_CAFE,
    CATEGORY_CHICKEN,
    CATEGORY_KOREAN,
    CATEGORY_CHINESE,
    CATEGORY_BUNSIK,
    CATEGORY_PIZZA_WESTERN,
    CATEGORY_BURGER,
    CATEGORY_JAPANESE_DONKATSU,
    CATEGORY_STEW_SOUP,
    CATEGORY_SUSHI,
    CATEGORY_JOKBAL_BOSSAM,
    CATEGORY_MEAT_GRILL,
    CATEGORY_SANDWICH,
    CATEGORY_SALAD,
    CATEGORY_LUNCHBOX_PORRIDGE,
    CATEGORY_ASIAN,
    CATEGORY_SINGLE,
    CATEGORY_FRANCHISE,
    CATEGORY_NEW,
    CATEGORY_LATE_NIGHT
};


// 주문 상태 열거형
enum ORDER_STATUS {
    ORDER_CHECK,
    ORDER_OK,
    ORDER_ING,
    ORDER_COMPLETE,
    ORDER_NO
};

#endif // STRUCT_AND_ENUM_H
