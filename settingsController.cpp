#include "settingsController.h"
#include "math.h"

QPoint SettingsController::previousMousePosition(-1, -1);

void SettingsController::settingChanged(QString name, QVariant newValue)
{
    if ( name == "UpdateType" ) {
        QString v = newValue.toString();
        emit updateTypeChanged(v == "Freeze" ? ScopeChannelController::SingleShot : ScopeChannelController::Periodically);
    }
    if ( name == "AcquisitionTime" ) {
        emit updateIntervalChanged(newValue.toInt());
    }
    if ( name.endsWith("_mode") ) {
        QString channel = name.remove("_mode");
        channel[0] = channel[0].toUpper();
        emit channelModeChanged(channel, newValue.toString());
    }
}

void SettingsController::dataRangeChangeCompleted()
{
    previousMousePosition = QPoint(-1, -1);
}

void SettingsController::dataRangeChangeRequested(QString channel, QString strAxis, QString strKind, int newMouseX, int newMouseY)
{
    QPoint newMousePosition(newMouseX, newMouseY);
    if ( previousMousePosition == QPoint(-1, -1) ) {
        previousMousePosition = newMousePosition;
        return;
    }
    const QPoint change = newMousePosition - previousMousePosition;
    Channel::Axis axis = strAxis == "x" ? Channel::HorizontalAxis : Channel::VerticalAxis;
    float distance = 0;
    if ( axis == Channel::HorizontalAxis ) {
        distance = change.x();
    }
    else {
        distance = change.y();
    }
    Channel::TransformationKind kind = strKind == "move" ? Channel::Movement : Channel::Scale;
    emit dataRangeChangeRequested(channel, kind, axis, distance);
    previousMousePosition = newMousePosition;
}