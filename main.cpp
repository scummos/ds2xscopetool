#include <QApplication>
#include <QDeclarativeView>
#include <QDeclarativeEngine>
#include <QDeclarativeContext>

#include "unistd.h"

#include "plotline.h"
#include "deviceCommunicationThread.h"
#include "channelController.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

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

    QDeclarativeItem* chartline = qobject_cast<QDeclarativeItem*>(component.create());
    chartline->setParentItem(rootObj);
    Plotline* line = qobject_cast<Plotline*>(chartline);
    for ( int i = 0; i < 3000; i++ ) {
        line->data->data[i] = cos(i / 200.0) * 100;
    }
    line->setDataRange(QRectF(0, -150, 1000, 300));
    QMetaObject::invokeMethod(chartline, "setup");
//     ScopeChannelController* controller  = new ScopeChannelController(chartline, worker);
//     controller->setUpdateType(ChannelController::Periodically);


    QDeclarativeItem* anotherLine = qobject_cast<QDeclarativeItem*>(component.create());
    anotherLine->setParentItem(rootObj);
    anotherLine->setProperty("color", "yellow");

    line = qobject_cast<Plotline*>(anotherLine);
    for ( int i = 0; i < 3000; i++ ) {
        line->data->data[i] = sin(i / 200.0) * 100;
    }
    line->setDataRange(QRectF(0, -150, 1000, 300));
    QMetaObject::invokeMethod(anotherLine, "setup");
    QDeclarativeItem* textArea =  view.rootObject()->findChild<QDeclarativeItem*>("jsChannel");
    QList<Plotline*> lines;
    lines << qobject_cast<Plotline*>(chartline) << qobject_cast<Plotline*>(anotherLine);
    JSDefinedChannelController* jsController = new JSDefinedChannelController(anotherLine, textArea, lines);

    view.setGeometry(100, 100, 800, 480);
    view.show();

    return a.exec();
}