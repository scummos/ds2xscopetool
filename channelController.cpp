#include "channelController.h"
#include "utils.h"
#include "settingsController.h"
#include "deviceCommunicationWorker.h"
#include "plotline.h"

#include <QDateTime>

ChannelController::ChannelController(QDeclarativeItem* curve)
    : curve(qobject_cast<PlotLine*>(curve))
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
    resetTimer();
}

void ChannelController::resetTimer()
{
    if ( updateType == Periodically ) {
        updateTimer.start();
    }
}

void ChannelController::changeChannelMode(QString channel, QString newMode)
{
    if ( curve->property("id") == channel ) {
        if ( newMode.toLower() == "off" ) {
            qDebug() << "disabling channel" << channel;
            curve->enabled = false;
        }
        else {
            curve->enabled = true;
            resetTimer();
        }
    }
    redraw();
}

void ChannelController::connectToSettingsController(const SettingsController* controller)
{
    QObject::connect(controller, SIGNAL(updateTypeChanged(ChannelController::UpdateType)),
                     this, SLOT(setUpdateType(ChannelController::UpdateType)));
    QObject::connect(controller, SIGNAL(updateIntervalChanged(int)),
                     this, SLOT(setUpdateInterval(int)));
    QObject::connect(controller, SIGNAL(dataRangeChangeRequested(QString, Channel::TransformationKind, Channel::Axis, float)),
                     this, SLOT(changeDataRange(QString, Channel::TransformationKind, Channel::Axis, float)));
    QObject::connect(controller, SIGNAL(channelModeChanged(QString,QString)),
                     this, SLOT(changeChannelMode(QString,QString)));
    QObject::connect(controller, SIGNAL(autoRangeRequested()),
                     this, SLOT(autoDataRange()));
}

void ChannelController::autoDataRange()
{
    if ( curve->data->data.size() < 2 ) {
        // nothing to scale
        return;
    }
    else {
        // scale the curve to fit on 1/8 of the screen vertically
        // and on the whole screen horizontally
        const float width = curve->data->data.count() - 1;
        // ignore the first data point to make scaling work nicely for FFT + DC offset signals
        float ymin = curve->data->data[1], ymax = curve->data->data[1];
        foreach ( const float value, curve->data->data.values().mid(1) ) {
            if ( value < ymin ) ymin = value;
            if ( value > ymax ) ymax = value;
        }
        const float height = ymax-ymin;
        const QRectF newRange = QRectF(0, ymin - (8-1.8*curve->getChannelNumber())*height, width, height * 8);
        curve->setDataRange(newRange);
    }
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

void ScopeChannelController::changeChannelMode(QString channel, QString newMode)
{
    ChannelController::changeChannelMode(channel, newMode);
    if ( curve->property("id") == channel ) {
        if ( newMode == "Fake" ) {
            fakeMode = true;
        }
        else {
            fakeMode = false;
            resetTimer();
        }
    }
}

void ScopeChannelController::fillCurveWithFakeData()
{
    for ( int i = 0; i < 1400; i++ ) {
        curve->data->data[i] = cos(i/30.0) * curve->getDataRange().height() / 8.0 + curve->getDataRange().top() + curve->getDataRange().height() / 2.0;
    }
    ChannelController::redraw();
}

void ScopeChannelController::doSingleUpdate()
{
    if ( ! curve->enabled ) {
        return;
    }
    if ( fakeMode ) {
        fillCurveWithFakeData();
        return;
    }
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
    resetTimer();
}

JSDefinedChannelController::JSDefinedChannelController(QDeclarativeItem* curve, QDeclarativeItem* textArea, QList< PlotLine* > inputChannels)
    : ChannelController(curve)
    , inputChannels(inputChannels)
    , textArea(textArea)
{
    QObject::connect(textArea, SIGNAL(textChanged(const QString&)), this, SLOT(doUpdate(const QString&)));
    foreach ( const PlotLine* chan, inputChannels ) {
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

FixedFunctionChannelController::FixedFunctionChannelController(QDeclarativeItem* curve, const QList<PlotLine*>& channels)
    : ChannelController(curve)
    , channels(channels)
    , operationMode(CrossCorrelation)
    , fftTargetChannel(0)
{
    updateTimer.setInterval(75);
    updateTimer.setSingleShot(true);
    QObject::connect(&updateTimer, SIGNAL(timeout()), this, SLOT(doUpdate()));
    foreach ( PlotLine* channel, channels ) {
        QObject::connect(channel, SIGNAL(dataChanged()), this, SLOT(scheduleUpdate()));
    }
}

void FixedFunctionChannelController::scheduleUpdate()
{
    // Only restart the timer if it's not running, because otherwise
    // frequent changes in the data will prevent updates
    if ( ! updateTimer.isActive() ) {
        updateTimer.start();
    }
}

void FixedFunctionChannelController::changeChannelMode(QString channel, QString newMode)
{
    if ( curve->property("id") == channel ) {
        bool autoRange = false;
        if ( newMode.startsWith("FFT") ) {
            // only do auto range if coming from another operation mode
            autoRange = operationMode != FourierTransform;
            setOperationMode(FourierTransform);
            bool ok = false;
            int channel = newMode.split(" ").last().toLower().replace("ch", "").toInt(&ok) - 1;
            if ( ok && channel < channels.count() ) {
                fftTargetChannel = channels.at(channel);
            }
            else {
                fftTargetChannel = 0;
            }
            doUpdate();
        }
        else if ( newMode == "CrossCorr" ) {
            setOperationMode(CrossCorrelation);
            autoRange = true;
        }
        doUpdate();
        if ( autoRange ) {
            autoDataRange();
        }
    }
    ChannelController::changeChannelMode(channel, newMode);
}

void FixedFunctionChannelController::setOperationMode(FixedFunctionChannelController::OperationMode newMode)
{
    operationMode = newMode;
}

void FixedFunctionChannelController::doUpdate()
{
    if ( operationMode == CrossCorrelation && channels.count() >= 2 ) {
        curve->data->data = Utils::crossCorrelation(channels[0]->data->data, channels[1]->data->data);
    }
    if ( operationMode == FourierTransform && fftTargetChannel ) {
        curve->data->data = Utils::fastFourierTransform(fftTargetChannel->data->data);
    }
    redraw();
}



