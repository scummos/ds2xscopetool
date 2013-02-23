QT       += core gui declarative

TARGET = qmlapp
TEMPLATE = app

SOURCES += main.cpp abstracttmcdevice.cpp linuxtmcdevice.cpp deviceCommunicationThread.cpp channelController.cpp settingsController.cpp
HEADERS  += plotline.h linuxtmcdevice.h abstracttmcdevice.h deviceCommunicationThread.h channelController.h utils.h settingsController.h
OTHER_FILES += ui.qml
QMAKE_CXXFLAGS += -std=c++0x -g
