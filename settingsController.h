#ifndef SETTINGSCONTROLLER_H
#define SETTINGSCONTROLLER_H
#include <QObject>
#include <qvariant.h>
#include "channelController.h"

class SettingsController : public QObject {
Q_OBJECT
public slots:
    void settingChanged(QString name, QVariant newValue);
signals:
    void updateIntervalChanged(int newInterval);
    void updateTypeChanged(ChannelController::UpdateType newType);
    void acquisitionTypeChanged(ScopeChannelController::AcquisitionType newType);
};

#endif