QT       += core gui
QT       += core gui widgets network
QT       += sql
QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
TEMPLATE  = app
TARGET    = WHAT_rider

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    delivery_widget.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    delivery_widget.h \
    mainwindow.h

FORMS += \
    delivery_widget.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    image.qrc
