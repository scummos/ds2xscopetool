#include "channelController.h"
#include <qdatetime.h>

ChannelController::ChannelController(QDeclarativeItem* curve)
    : curve(qobject_cast<Plotline*>(curve))
    , updateInterval(50)
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

void ChannelController::doSingleUpdate()
{
    curve->slotDataChanged();
}

ScopeChannelController::ScopeChannelController(QDeclarativeItem* curve, DeviceCommunicationWorker* worker)
    : ChannelController(curve)
    , worker(worker)
{
    Q_ASSERT(worker && "must provide a valid worker thread");
}

void ScopeChannelController::doSingleUpdate()
{
    curve->data->data[20] = QDateTime::currentMSecsSinceEpoch() % 11;
    qDebug() << "updating curve data" << QDateTime::currentMSecsSinceEpoch() % 11;
    ChannelController::doSingleUpdate();
}
