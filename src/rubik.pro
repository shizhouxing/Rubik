#-------------------------------------------------
#
# Project created by QtCreator 2016-11-07T15:43:36
#
#-------------------------------------------------

QT       += core gui opengl

win32: LIBS += -lOpengl32

CONFIG += C++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = rubik
TEMPLATE = app

SOURCES += main.cpp\
        rubik.cpp \
    state.cpp \
    solver.cpp \
    rubikgl.cpp \
    custominputdialog.cpp

HEADERS  += rubik.h \
    state.h \
    solver.h \
    rubikgl.h \
    custominputdialog.h

FORMS    += rubik.ui \
    custominputdialog.ui

RESOURCES += \
    rubik.qrc
