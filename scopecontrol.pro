QT       += core gui declarative

TARGET = qmlapp
TEMPLATE = app

SOURCES += main.cpp
HEADERS  += plotline.h
OTHER_FILES += ui.qml
QMAKE_CXXFLAGS += -std=c++0x
