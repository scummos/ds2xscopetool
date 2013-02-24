#include "channelController.h"
#include "utils.h"
#include "settingsController.h"

#include <qdatetime.h>

ChannelController::ChannelController(QDeclarativeItem* curve)
    : curve(qobject_cast<Plotline*>(curve))
    , updateType(SingleShot)
    , updateInterval(50)
{
    Q_ASSERT(curve && "must pass a PlotLine object as curve");
    updateTimer.setSingleShot(true);
}

void ChannelController::setUpdateInterval(int msecs)
{
    Q_ASSERT(updateInterval > 0 && "update interval must be positive");
    updateInterval = msecs;
    updateTimer.stop();
    updateTimer.setInterval(updateInterval);
    updateTimer.start();
}

void ChannelController::setUpdateType(ChannelController::UpdateType type)
{
    updateType = type;
    if ( updateType == Periodically ) {
        updateTimer.start();
    }
}

void ChannelController::connectToSettingsController(const SettingsController* controller)
{
    QObject::connect(controller, SIGNAL(updateTypeChanged(ChannelController::UpdateType)),
                     this, SLOT(setUpdateType(ChannelController::UpdateType)));
    QObject::connect(controller, SIGNAL(updateIntervalChanged(int)),
                     this, SLOT(setUpdateInterval(int)));
}

void ChannelController::redraw()
{
    curve->notifyDataChanged();
}

ScopeChannelController::ScopeChannelController(QDeclarativeItem* curve, DeviceCommunicationWorker* worker)
    : ChannelController(curve)
    , channel("CHAN1")
    , worker(worker)
{
    Q_ASSERT(worker && "must provide a valid worker thread");
    QObject::connect(&updateTimer, SIGNAL(timeout()), this, SLOT(doSingleUpdate()));
}

void ScopeChannelController::doSingleUpdate()
{
    ReadChannelDataCommunicationRequest* req = new ReadChannelDataCommunicationRequest(this, "updateReady");
    req->channel = channel;
    worker->enqueue(req);
}

void ScopeChannelController::updateReady(CommunicationReply* reply)
{
    ScopeChannelDataCommunicationReply* scopeReply = static_cast<ScopeChannelDataCommunicationReply*>(reply);
    curve->data->data = Utils::parseScopeChannelReply(reply->reply, scopeReply->yref, scopeReply->scale, scopeReply->offset);
    delete reply;
    ChannelController::redraw();
    if ( updateType == Periodically ) {
        updateTimer.start();
    }
}

JSDefinedChannelController::JSDefinedChannelController(QDeclarativeItem* curve, QDeclarativeItem* textArea, QList< Plotline* > inputChannels)
    : ChannelController(curve)
    , inputChannels(inputChannels)
    , textArea(textArea)
{
    QObject::connect(textArea, SIGNAL(textChanged(const QString&)), this, SLOT(doUpdate(const QString&)));
    foreach ( const Plotline* chan, inputChannels ) {
        QObject::connect(chan, SIGNAL(dataChanged()), this, SLOT(doUpdate()));
        QObject::connect(chan, SIGNAL(dataRangeChanged()), this, SLOT(doUpdate()));
    }
    doUpdate(QString::null);
}

void JSDefinedChannelController::doUpdate()
{
    QString text = textArea->property("text").toString();
    doUpdate(text);
}

void JSDefinedChannelController::doUpdate(const QString& text)
{
    QVariant returnedValue;
    float y1, y2;
    QTime t;
    t.start();
    for ( int i = 0; i < qMin(inputChannels[0]->data->data.size(), inputChannels[1]->data->data.size()); i++ ) {
        y1 = inputChannels[0]->data->data[i];
        y2 = inputChannels[1]->data->data[i];
        QMetaObject::invokeMethod(textArea, "doCalculation", Q_RETURN_ARG(QVariant, returnedValue),
                                  Q_ARG(QVariant, y1), Q_ARG(QVariant, y2), Q_ARG(QVariant, text));
        curve->data->data[i] = returnedValue.toFloat();
    }
//     qDebug() << "JS function evaluation took " << t.elapsed() << "ms";
    ChannelController::redraw();
}

FixedFunctionChannelController::FixedFunctionChannelController(QDeclarativeItem* curve, Plotline* channel1, Plotline* channel2)
    : ChannelController(curve)
    , channel1(channel1)
    , channel2(channel2)
{

}

void FixedFunctionChannelController::setOperationMode(FixedFunctionChannelController::OperationMode newMode)
{
    operationMode = newMode;
}

void FixedFunctionChannelController::doUpdate()
{
    curve->data->data = Utils::crossCorrelation(channel1->data->data, channel2->data->data);
}



