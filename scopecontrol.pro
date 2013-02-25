QT       += core gui declarative

TARGET = qmlapp
TEMPLATE = app
LIBS += -litpp

SOURCES += main.cpp abstracttmcdevice.cpp linuxtmcdevice.cpp deviceCommunicationWorker.cpp channelController.cpp settingsController.cpp
HEADERS  += plotline.h linuxtmcdevice.h abstracttmcdevice.h deviceCommunicationWorker.h channelController.h utils.h settingsController.h
OTHER_FILES += ui.qml
QMAKE_CXXFLAGS += -std=c++0x -g
