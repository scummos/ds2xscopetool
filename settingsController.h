#ifndef SETTINGSCONTROLLER_H
#define SETTINGSCONTROLLER_H
#include <QObject>
#include <qvariant.h>
#include "channelController.h"

class SettingsController : public QObject {
Q_OBJECT
public slots:
    void settingChanged(QString name, QVariant newValue);
    void dataRangeChangeRequested(QString channel, QString axis, QString kind, int newMouseX, int newMouseY);
    void dataRangeChangeCompleted();
signals:
    void updateIntervalChanged(int newInterval);
    void updateTypeChanged(ChannelController::UpdateType newType);
    void acquisitionTypeChanged(ScopeChannelController::AcquisitionType newType);
    void dataRangeChangeRequested(QString channel, Channel::TransformationKind kind, Channel::Axis axis, float magnitude);
    void channelModeChanged(QString channel, QString newMode);
private:
    static QPoint previousMousePosition;
};

#endif