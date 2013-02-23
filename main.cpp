#include <QApplication>
#include <QDeclarativeView>
#include <QDeclarativeEngine>
#include <QDeclarativeContext>

#include "unistd.h"

#include "plotline.h"
#include "deviceCommunicationThread.h"
#include "channelController.h"
#include "settingsController.h"

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
    QDeclarativeContext* context = engine->rootContext();
    QDeclarativeItem* rootObj = view.rootObject()->findChild<QDeclarativeItem*>("mychart");

    QDeclarativeComponent component(engine, QUrl::fromLocalFile("ChartLine.qml"));

    QDeclarativeItem* channel1 = qobject_cast<QDeclarativeItem*>(component.create());
    channel1->setParentItem(rootObj);
    qobject_cast<Plotline*>(channel1)->setDataRange(QRectF(0, -150, 1000, 300));
    QMetaObject::invokeMethod(channel1, "setup");
    channel1->setProperty("color", "#FFC800");
    ScopeChannelController* controller = new ScopeChannelController(channel1, worker);
    controller->channel = "CHAN1";
    controller->setUpdateType(ChannelController::Periodically);

    QDeclarativeItem* channel2 = qobject_cast<QDeclarativeItem*>(component.create());
    channel2->setParentItem(rootObj);
    qobject_cast<Plotline*>(channel2)->setDataRange(QRectF(0, -150, 1000, 300));
    QMetaObject::invokeMethod(channel2, "setup");
    channel2->setProperty("color", "#1C73FF");
    ScopeChannelController* controller2 = new ScopeChannelController(channel2, worker);
    controller2->channel = "CHAN2";
    controller2->setUpdateType(ChannelController::Periodically);

    QDeclarativeItem* jsMathLine = qobject_cast<QDeclarativeItem*>(component.create());
    jsMathLine->setParentItem(rootObj);
    qobject_cast<Plotline*>(jsMathLine)->setDataRange(QRectF(0, -150, 1000, 300));
    QMetaObject::invokeMethod(jsMathLine, "setup");
    jsMathLine->setProperty("color", "#61E000");
    QDeclarativeItem* textArea = view.rootObject()->findChild<QDeclarativeItem*>("jsChannel");
    QList<Plotline*> lines;
    lines << qobject_cast<Plotline*>(channel1) << qobject_cast<Plotline*>(channel2);
    JSDefinedChannelController* jsController = new JSDefinedChannelController(jsMathLine, textArea, lines);

    view.setGeometry(100, 100, 800, 480);
    view.show();

    return a.exec();
}