#-------------------------------------------------
#
# Project created by QtCreator 2017-08-10T10:06:33
#
#-------------------------------------------------

QT       += core gui
QT       += serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = test2
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    spcomm.cpp

HEADERS  += mainwindow.h \
    spcomm.h

FORMS    += mainwindow.ui
