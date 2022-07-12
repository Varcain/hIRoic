QT += core gui widgets serialport concurrent

TARGET = irloader_panel
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    serialhandler.cpp \
    mainpanel.cpp \
    protocolparser.cpp

HEADERS += \
    serialhandler.h \
    config.h \
    mainpanel.h \
    protocolparser.h

