#include "channelController.h"
#include "utils.h"
#include <qdatetime.h>

ChannelController::ChannelController(QDeclarativeItem* curve)
    : curve(qobject_cast<Plotline*>(curve))
    , updateInterval(75)
    , updateType(SingleShot)
{
    Q_ASSERT(curve && "must pass a PlotLine object as curve");
}

void ChannelController::setUpdateInterval(int msecs)
{
    Q_ASSERT(updateInterval > 0 && "update interval must be positive");
    updateInterval = msecs;
}

void ChannelController::setUpdateType(ChannelController::UpdateType type)
{
    updateType = type;
    if ( type == Periodically ) {
        updateTimer.setSingleShot(false);
        updateTimer.setInterval(updateInterval);
        QObject::connect(&updateTimer, SIGNAL(timeout()), this, SLOT(doSingleUpdate()));
        updateTimer.start();
    }
    else {
        updateTimer.stop();
    }
}

void ChannelController::redraw()
{
    curve->notifyDataChanged();
}

ScopeChannelController::ScopeChannelController(QDeclarativeItem* curve, DeviceCommunicationWorker* worker)
    : ChannelController(curve)
    , worker(worker)
    , channel("CHAN1")
{
    Q_ASSERT(worker && "must provide a valid worker thread");
}

void ScopeChannelController::doSingleUpdate()
{
    ReadChannelDataCommunicationRequest* req = new ReadChannelDataCommunicationRequest(this, "updateReady");
    req->channel = channel;
    worker->enqueue(req);
}

void ScopeChannelController::updateReady(CommunicationReply* reply)
{
    curve->data->data = Utils::parseScopeChannelReply(reply->reply);
    delete reply;
    ChannelController::redraw();
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
    qDebug() << "JS function evaluation took " << t.elapsed() << "ms";
    ChannelController::redraw();
}


