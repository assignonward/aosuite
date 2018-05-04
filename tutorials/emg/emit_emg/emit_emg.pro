DEPTH = ../../..
include($${DEPTH}/qamqp.pri)

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app
INCLUDEPATH += $${QAMQP_INCLUDEPATH}
LIBS += -L$${DEPTH}/src $${QAMQP_LIBS}
macx:CONFIG -= app_bundle

SOURCES += main.cpp \
    mainwindow.cpp \
    emgemitter.cpp

HEADERS += \
    mainwindow.h \
    emgemitter.h

FORMS += \
    mainwindow.ui
