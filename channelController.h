#ifndef CHANNELCONTROLLER_H
#define CHANNELCONTROLLER_H

#include <QDeclarativeItem>
#include <QTimer>

#include "deviceCommunicationThread.h"
#include "plotline.h"

class ChannelController : public QObject {
Q_OBJECT
public:
    enum UpdateType {
        SingleShot,
        Periodically
    };

    ChannelController(QDeclarativeItem* curve);
    void setUpdateType(UpdateType type);
    void setUpdateInterval(int msecs);

public slots:
    // Make sure to call this default implementation from derived classes,
    // after the code of the derived class has run
    void redraw();

protected:
    Plotline* curve;

private:
    UpdateType updateType;
    // the update interval, in milliseconds
    int updateInterval;
    QTimer updateTimer;
};

class ScopeChannelController : public ChannelController {
Q_OBJECT
public:
    ScopeChannelController(QDeclarativeItem* curve, DeviceCommunicationWorker* worker);
    QString channel;
public slots:
    virtual void doSingleUpdate();
    void updateReady(CommunicationReply* reply);
private:
    DeviceCommunicationWorker* worker;
};

class JSDefinedChannelController : public ChannelController {
Q_OBJECT
public:
    JSDefinedChannelController(QDeclarativeItem* curve, QDeclarativeItem* textArea, QList<Plotline*> inputChannels);
public slots:
    virtual void doUpdate(const QString& text);
private:
    QList<Plotline*> inputChannels;
    QDeclarativeItem* textArea;
};

#endif