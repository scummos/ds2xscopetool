#ifndef CHANNELCONTROLLER_H
#define CHANNELCONTROLLER_H

#include <QDeclarativeItem>
#include <QTimer>

#include "deviceCommunicationThread.h"
#include "plotline.h"

class SettingsController;

class ChannelController : public QObject {
Q_OBJECT
public:
    enum UpdateType {
        SingleShot,
        Periodically
    };

    ChannelController(QDeclarativeItem* curve);
    virtual void connectToSettingsController(const SettingsController* controller);
public slots:
    void setUpdateType(ChannelController::UpdateType type);
    void setUpdateInterval(int msecs);

    // Make sure to call this default implementation from derived classes,
    // after the code of the derived class has run
    void redraw();

protected:
    Plotline* curve;
    QTimer updateTimer;
    UpdateType updateType;

private:
    // the update interval, in milliseconds
    int updateInterval;
};

class ScopeChannelController : public ChannelController {
Q_OBJECT
public:
    enum AcquisitionType {
        Displayed,
        Full
    };

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
    virtual void doUpdate();
private:
    QList<Plotline*> inputChannels;
    QDeclarativeItem* textArea;
};

class FixedFunctionChannelController : public ChannelController {
Q_OBJECT
public:
    FixedFunctionChannelController(QDeclarativeItem* curve, Plotline* channel1, Plotline* channel2);
    enum OperationMode {
        CrossCorrelation,
        FourierTransform
    };
public slots:
    virtual void doUpdate();
    virtual void setOperationMode(FixedFunctionChannelController::OperationMode newMode);
private:
    Plotline* channel1;
    Plotline* channel2;
    OperationMode operationMode;
};

#endif