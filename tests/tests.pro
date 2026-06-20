QT += core multimedia testlib
QT -= gui

CONFIG += testcase console c++11
CONFIG -= app_bundle

TEMPLATE = app
TARGET = tst_engine

INCLUDEPATH += ../src

SOURCES += \
    main.cpp \
    tst_timedexercise.cpp \
    tst_exerciselistmodel.cpp \
    tst_exercisetimer.cpp \
    ../src/timedexercise.cpp \
    ../src/exerciselistmodel.cpp \
    ../src/exercisetimer.cpp \
    soundplayer_stub.cpp

HEADERS += \
    tst_timedexercise.h \
    tst_exerciselistmodel.h \
    tst_exercisetimer.h \
    ../src/timedexercise.h \
    ../src/exerciselistmodel.h \
    ../src/exercisetimer.h \
    ../src/soundplayer.h \
    ../src/eoqttrace.h
