#include "settingsController.h"

void SettingsController::settingChanged(QString name, QVariant newValue)
{
    qDebug() << "setting changed:" << name << newValue;
    if ( name == "UpdateType" ) {
        QString v = newValue.toString();
        emit updateTypeChanged(v == "Freeze" ? ScopeChannelController::SingleShot : ScopeChannelController::Periodically);
    }
}