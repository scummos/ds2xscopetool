#include "channelController.h"
#include "utils.h"
#include "settingsController.h"
#include "deviceCommunicationWorker.h"

#include <QDateTime>

ChannelController::ChannelController(QDeclarativeItem* curve)
    : curve(qobject_cast<Plotline*>(curve))
    , updateType(SingleShot)
    , updateInterval(50)
{
    Q_ASSERT(curve && "must pass a PlotLine object as curve");
    updateTimer.setSingleShot(true);
    updateTimer.setInterval(updateInterval);
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
    QObject::connect(controller, SIGNAL(dataRangeChangeRequested(QString, Channel::TransformationKind, Channel::Axis, float)),
                     this, SLOT(changeDataRange(QString, Channel::TransformationKind, Channel::Axis, float)));
}

void ChannelController::changeDataRange(QString channel, Channel::TransformationKind kind, Channel::Axis axis, float amount)
{
    if ( curve->property("id") == channel ) {
        QRectF range = curve->getDataRange();
        if ( kind == Channel::Scale ) {
            if ( axis == Channel::HorizontalAxis ) {
                float change = range.width() * amount / 100;
                range.setLeft(range.left() - change / 2);
                range.setRight(range.right() + change / 2);
            }
            else {
                float change = range.height() * amount / 100;
                range.setTop(range.top() - change*range.top() / range.height());
                range.setBottom(range.bottom() - change*range.bottom() / range.height());
            }
        }
        else {
            if ( axis == Channel::HorizontalAxis ) {
                float adjusted = amount*range.width() / 600;
                range.setLeft(range.left() - adjusted);
                range.setRight(range.right() - adjusted);
            }
            else {
                float adjusted = amount*range.height() / 600;
                range.setTop(range.top() + adjusted);
                range.setBottom(range.bottom() + adjusted);
            }
        }
        curve->setDataRange(range);
    }
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
    , operationMode(CrossCorrelation)
{
    connect(channel1, SIGNAL(dataChanged()), this, SLOT(doUpdate()));
}

void FixedFunctionChannelController::setOperationMode(FixedFunctionChannelController::OperationMode newMode)
{
    operationMode = newMode;
}

void FixedFunctionChannelController::doUpdate()
{
    if ( operationMode == CrossCorrelation ) {
        curve->data->data = Utils::crossCorrelation(channel1->data->data, channel2->data->data);
    }
}



