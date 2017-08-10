#-------------------------------------------------
#
# Project created by QtCreator 2017-08-09T14:25:23
#
#-------------------------------------------------

QT       += core gui
QT       += serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = test1
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    spcomm.cpp

HEADERS  += mainwindow.h \
    spcomm.h

FORMS    += mainwindow.ui
