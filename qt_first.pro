#-------------------------------------------------
#
# Project created by QtCreator 2016-02-06T12:05:51
#
#-------------------------------------------------

QT       += core gui
#QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qt_first
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    csatan.cpp \
    dialog.cpp \
    searchbox.cpp \
    googlesuggest.cpp

HEADERS  += mainwindow.h \
    csatan.h \
    dialog.h \
    searchbox.h \
    googlesuggest.h

FORMS    += mainwindow.ui \
    dialog.ui

INCLUDEPATH += /home/devel/gomtv/include \

#LIBS += /home/devel/gomtv/lib/libgen.a \
LIBS += -L /home/devel/gomtv/lib -lmon -lcli -linet -lgen  \

CONFIG += console

DISTFILES +=
