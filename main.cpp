#include <QApplication>
#include <QDeclarativeView>
#include <QDeclarativeEngine>
#include <QDeclarativeContext>

#include "unistd.h"

#include "plotline.h"
#include "deviceCommunicationWorker.h"
#include "channelController.h"
#include "settingsController.h"
#include "utils.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qRegisterMetaType<CommunicationReply*>("CommunicationReply*");

    QThread* thread = new QThread();
    DeviceCommunicationWorker* worker = new DeviceCommunicationWorker();
    worker->moveToThread(thread);
    QObject::connect(thread, SIGNAL(started()), worker, SLOT(connectToDevice()));
    thread->start();

    qmlRegisterType<PlotLine>("CustomComponents", 1, 0, "PlotLine");

    QDeclarativeView view;
    view.setSource(QUrl("./ui.qml"));
    view.setResizeMode(QDeclarativeView::SizeRootObjectToView);

    QDeclarativeEngine* engine = view.engine();
    QDeclarativeItem* rootObj = view.rootObject()->findChild<QDeclarativeItem*>("mychart");
    SettingsController* settingsController = new SettingsController();
    QObject::connect(view.rootObject()->findChild<QObject*>("menuButtonList"), SIGNAL(paramChanged(QString, QVariant)),
                     settingsController, SLOT(settingChanged(QString, QVariant)));
    QObject* dataRangeManager = view.rootObject()->findChild<QObject*>("dataRangeManager");
    QObject::connect(dataRangeManager, SIGNAL(dataRangeChangeRequested(QString, QString, QString, int, int)),
                     settingsController, SLOT(dataRangeChangeRequested(QString, QString, QString, int, int)));
    QObject::connect(dataRangeManager, SIGNAL(dataRangeChangeCompleted()),
                     settingsController, SLOT(dataRangeChangeCompleted()));
    QObject::connect(view.rootObject(), SIGNAL(autoRangeRequested()),
                     settingsController, SIGNAL(autoRangeRequested()));

    QDeclarativeComponent component(engine, QUrl::fromLocalFile("ChartLine.qml"));


    PlotLine* channel1 = qobject_cast<PlotLine*>(component.create());
    channel1->setProperty("color", "#FFC800");
    channel1->setParentItem(rootObj);
    channel1->setChannelNumber(1);
    channel1->data->channelType = Channel::ScopeChannelType;
    QMetaObject::invokeMethod(channel1, "setup");
    ScopeChannelController* controller = new ScopeChannelController(channel1, worker);
    controller->channel = "CHAN1";
    controller->setUpdateType(ChannelController::Periodically);
    controller->connectToSettingsController(settingsController);

    PlotLine* channel2 = qobject_cast<PlotLine*>(component.create());
    channel2->setParentItem(rootObj);
    channel2->setChannelNumber(2);
    channel2->data->channelType = Channel::ScopeChannelType;
    QMetaObject::invokeMethod(channel2, "setup");
    channel2->setProperty("color", "#1C73FF");
    ScopeChannelController* controller2 = new ScopeChannelController(channel2, worker);
    controller2->channel = "CHAN2";
    controller2->setUpdateType(ChannelController::Periodically);
    controller2->connectToSettingsController(settingsController);

    PlotLine* jsMathLine = qobject_cast<PlotLine*>(component.create());
    jsMathLine->setParentItem(rootObj);
    jsMathLine->setChannelNumber(3);
    jsMathLine->data->channelType = Channel::MathChannelType;
    QMetaObject::invokeMethod(jsMathLine, "setup");
    jsMathLine->setProperty("color", "#61E000");
    QDeclarativeItem* textArea = view.rootObject()->findChild<QDeclarativeItem*>("jsChannel");
    QList<PlotLine*> lines;
    lines << channel1 << channel2;
    JSDefinedChannelController* jsController = new JSDefinedChannelController(jsMathLine, textArea, lines);
    jsController->connectToSettingsController(settingsController);

    PlotLine* fixedMathLine = qobject_cast<PlotLine*>(component.create());
    fixedMathLine->setParentItem(rootObj);
    fixedMathLine->setChannelNumber(4);
    fixedMathLine->data->channelType = Channel::MathChannelType;
    QMetaObject::invokeMethod(fixedMathLine, "setup");
    fixedMathLine->setProperty("color", "#FF4498");
    FixedFunctionChannelController* fixedController = new FixedFunctionChannelController(fixedMathLine, QList<PlotLine*>() << lines << jsMathLine);
    fixedController->connectToSettingsController(settingsController);

    view.setGeometry(100, 100, 800, 480);
    view.show();

    return a.exec();
}