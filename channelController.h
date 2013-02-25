#ifndef CHANNELCONTROLLER_H
#define CHANNELCONTROLLER_H

#include <QDeclarativeItem>
#include <QTimer>

#include "plotline.h"

class SettingsController;
class CommunicationReply;
class DeviceCommunicationWorker;

class ChannelController : public QObject {
Q_OBJECT
public:
    enum UpdateType {
        SingleShot,
        Periodically
    };

    ChannelController(QDeclarativeItem* curve);
    virtual void connectToSettingsController(const SettingsController* controller);
    void resetTimer();

public slots:
    void setUpdateType(ChannelController::UpdateType type);
    void setUpdateInterval(int msecs);
    void changeDataRange(QString channel, Channel::TransformationKind kind, Channel::Axis axis, float amount);
    void autoDataRange();
    virtual void changeChannelMode(QString channel, QString newMode);

    // Make sure to call this default implementation from derived classes,
    // after the code of the derived class has run
    void redraw();

protected:
    PlotLine* curve;
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
    virtual void changeChannelMode(QString channel, QString newMode);
    QString channel;
    bool fakeMode;

public slots:
    virtual void doSingleUpdate();
    void updateReady(CommunicationReply* reply);
private:
    void fillCurveWithFakeData();
    DeviceCommunicationWorker* worker;
};

class JSDefinedChannelController : public ChannelController {
Q_OBJECT
public:
    JSDefinedChannelController(QDeclarativeItem* curve, QDeclarativeItem* textArea, QList<PlotLine*> inputChannels);
public slots:
    virtual void doUpdate(const QString& text);
    virtual void doUpdate();
private:
    QList<PlotLine*> inputChannels;
    QDeclarativeItem* textArea;
};

class FixedFunctionChannelController : public ChannelController {
Q_OBJECT
public:
    FixedFunctionChannelController(QDeclarativeItem* curve, const QList<PlotLine*>& channels);
    enum OperationMode {
        CrossCorrelation,
        FourierTransform
    };
    virtual void changeChannelMode(QString channel, QString newMode);
public slots:
    virtual void scheduleUpdate();
    virtual void doUpdate();
    virtual void setOperationMode(FixedFunctionChannelController::OperationMode newMode);
private:
    QList<PlotLine*> channels;
    OperationMode operationMode;
    QTimer updateTimer;
    PlotLine* fftTargetChannel;;
};

#endif