#-------------------------------------------------
#
# Project created by QtCreator 2015-08-02T11:10:04
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = calibration_software
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    calibraon.cpp

HEADERS  += mainwindow.h \
    calibraon.h

FORMS    += mainwindow.ui

INCLUDEPATH += E:/opencv/include/ \
               E:/opencv/include/opencv/ \
               E:/opencv/include/opencv2/


#LIBS += D:/opencv/lib/*.lib

CONFIG(debug,debug|release) {
LIBS += E:/opencv/lib/*d.lib
} else {
LIBS += E:/opencv/lib/*.lib}
