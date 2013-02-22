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
    QDeclarativeItem* rootObj = context->findChild<QDeclarativeItem*>("mychart");
    Plotline* curve = new Plotline();
    curve->setParentItem(rootObj);
    curve->setProperty("color", "red");
//     curve->setProperty("anchors.fill", "parent");
    curve->setProperty("width", 300);
    curve->setProperty("height", 300);
    view.scene()->addItem(qobject_cast<QDeclarativeItem*>(curve));

    view.setGeometry(100, 100, 800, 480);
    view.show();
    return a.exec();
}