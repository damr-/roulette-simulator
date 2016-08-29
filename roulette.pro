#-------------------------------------------------
#
# Project created by QtCreator 2016-08-04T09:25:58
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = roulette
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    qcustomplot.cpp \
    outputitemwidget.cpp

HEADERS  += mainwindow.h \
    qcustomplot.h \
    roulette.rc \
    outputitemwidget.h

FORMS    += mainwindow.ui \
    outputitemwidget.ui

RESOURCES += \
    resources.qrc

RC_FILE = roulette.rc
