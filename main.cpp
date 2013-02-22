#include "plotline.h"
#include <QApplication>
#include <QDeclarativeView>
#include <QDeclarativeEngine>
#include <QDeclarativeContext>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

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
    view.scene()->addItem(qobject_cast<QDeclarativeItem*>(chartline));
    QMetaObject::invokeMethod(chartline, "setup");

    view.setGeometry(100, 100, 800, 480);
    view.show();

    return a.exec();
}