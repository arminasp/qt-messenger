QT += core network sql
QT -= gui

CONFIG += c++11

TARGET = Server
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    server.cpp \
    task_manager.cpp \
    client.cpp

HEADERS += \
    server.h \
    task_manager.h \
    client.h \
    ../common_includes/commands.h \
    ../common_includes/std.h
