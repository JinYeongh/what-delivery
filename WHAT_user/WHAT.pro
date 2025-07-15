QT       += core gui
QT       += core gui widgets network
QT       += sql
QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
TEMPLATE  = app
TARGET    = WHAT

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    dbmanager.cpp \
    main.cpp \
    mainwindow.cpp \
    menu_head.cpp \
    order_list_widget.cpp \
    rst_main_icon.cpp \
    sel_menu_info.cpp \
    selected_menu_widget.cpp \
    addressbtn.cpp \
    menu_normal.cpp

HEADERS += \
    dbmanager.h \
    mainwindow.h \
    menu_head.h \
    order_list_widget.h \
    rst_main_icon.h \
    sel_menu_info.h \
    selected_menu_widget.h \
    struct_and_enum.h \
    addressbtn.h \
    menu_normal.h

FORMS += \
    mainwindow.ui \
    menu_head.ui \
    order_list_widget.ui \
    rst_main_icon.ui \
    sel_menu_info.ui \
    selected_menu_widget.ui \
    addressbtn.ui \
    menu_normal.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    image.qrc

DISTFILES +=
