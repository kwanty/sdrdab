#-------------------------------------------------
#
# Project created by QtCreator 2016-04-08T15:17:09
#
#-------------------------------------------------

QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = sdrdab-gui
TEMPLATE = app


SOURCES += main.cc mainwindow.cc gui_scheduler.cc \
    user_conf.cc

HEADERS  += mainwindow.h gui_scheduler.h scheduler.h \
    user_conf.h


FORMS    += mainwindow.ui


INCLUDEPATH += /home/doszek/git/DAB-decoder-2016/src/sdrdab
INCLUDEPATH += /home/doszek/git/DAB-decoder-2016/src/rtlsdr/include
INCLUDEPATH += /home/doszek/git/DAB-decoder-2016/src/rscode
INCLUDEPATH += /home/doszek/git/DAB-decoder-2016/src/gtest/include
INCLUDEPATH += /usr/include/gstreamer-1.0
INCLUDEPATH += /usr/include/glib-2.0
INCLUDEPATH += /usr/include/
INCLUDEPATH += /usr/include/c++/4.8
INCLUDEPATH += /usr/include/c++/4.8/backward
INCLUDEPATH += /usr/include/x86_64-linux-gnu
INCLUDEPATH += /usr/include/x86_64-linux-gnu/c+/4.8
INCLUDEPATH += /usr/lib/gcc/x86_64-linux-gnu/4.8/include
INCLUDEPATH += /usr/lib/gcc/x86_64-linux-gnu/include-fixed
INCLUDEPATH += /usr/lib/x86_64-linux-gnu/glib-2.0/include



