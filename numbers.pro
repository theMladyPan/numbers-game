TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += \
    /usr/include/boost


SOURCES += \
        main.cpp


LIBS += -pthread
