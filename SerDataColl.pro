#-------------------------------------------------
#
# Project created by QtCreator 2017-01-31T13:32:27
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SerDataColl
TEMPLATE = app


SOURCES += main.cpp\
    mainwindow.cpp \
    serdatactrl.cpp \
    serdatactrlview.cpp

HEADERS  += mainwindow.h \
    serdatactrl.h \
    serdatactrlview.h

win32{
    RC_ICONS += binary-code.ico
    QMAKE_TARGET_COMPANY = "Codependable LLC"
    QMAKE_TARGET_PRODUCT = "SerDataColl"
    QMAKE_TARGET_DESCRIPTION = "High speed serial data logger"
    QMAKE_TARGET_COPYRIGHT = "Copyright (C) 2017 Codependable LLC"
}
