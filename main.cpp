#include <QApplication>
#include <QDeclarativeView>
#include <QDeclarativeEngine>
#include <QDeclarativeContext>

#include "unistd.h"

#include "plotline.h"
#include "deviceCommunicationThread.h"
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


    AskCommandCommunicationRequest* req = new AskCommandCommunicationRequest(0, "");
    req->commandString = "*IDN?";
    worker->enqueue(req);

    qmlRegisterType<Plotline>("CustomComponents", 1, 0, "PlotLine");

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

    QDeclarativeComponent component(engine, QUrl::fromLocalFile("ChartLine.qml"));


    QDeclarativeItem* channel1 = qobject_cast<QDeclarativeItem*>(component.create());
    channel1->setProperty("id", "Channel1");
    channel1->setParentItem(rootObj);
    qobject_cast<Plotline*>(channel1)->setDataRange(QRectF(0, -0.2, 1300, 0.4));
    QMetaObject::invokeMethod(channel1, "setup");
    channel1->setProperty("color", "#FFC800");
    ScopeChannelController* controller = new ScopeChannelController(channel1, worker);
    controller->channel = "CHAN1";
    controller->setUpdateType(ChannelController::Periodically);
    controller->connectToSettingsController(settingsController);

    QDeclarativeItem* channel2 = qobject_cast<QDeclarativeItem*>(component.create());
    channel2->setParentItem(rootObj);
    channel2->setProperty("id", "Channel2");
    qobject_cast<Plotline*>(channel2)->setDataRange(QRectF(0, -0.2, 1300, 0.4));
    QMetaObject::invokeMethod(channel2, "setup");
    channel2->setProperty("color", "#1C73FF");
    ScopeChannelController* controller2 = new ScopeChannelController(channel2, worker);
    controller2->channel = "CHAN2";
    controller2->setUpdateType(ChannelController::Periodically);
    controller2->connectToSettingsController(settingsController);

    QDeclarativeItem* jsMathLine = qobject_cast<QDeclarativeItem*>(component.create());
    jsMathLine->setParentItem(rootObj);
    jsMathLine->setProperty("id", "jsMathLine");
    qobject_cast<Plotline*>(jsMathLine)->setDataRange(QRectF(0, -1, 1300, 2));
    QMetaObject::invokeMethod(jsMathLine, "setup");
    jsMathLine->setProperty("color", "#61E000");
    QDeclarativeItem* textArea = view.rootObject()->findChild<QDeclarativeItem*>("jsChannel");
    QList<Plotline*> lines;
    lines << qobject_cast<Plotline*>(channel1) << qobject_cast<Plotline*>(channel2);
    new JSDefinedChannelController(jsMathLine, textArea, lines);

    QDeclarativeItem* fixedMathLine = qobject_cast<QDeclarativeItem*>(component.create());
    fixedMathLine->setParentItem(rootObj);
    fixedMathLine->setProperty("id", "fixedMathLine");
    qobject_cast<Plotline*>(fixedMathLine)->setDataRange(QRectF(0, -0.4, 1000, 2.0));
    QMetaObject::invokeMethod(fixedMathLine, "setup");
    fixedMathLine->setProperty("color", "#FF4498");
    new FixedFunctionChannelController(fixedMathLine, lines[0], lines[1]);

    view.setGeometry(100, 100, 800, 480);
    view.show();

    return a.exec();
}