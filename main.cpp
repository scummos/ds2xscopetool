#include "plotline.h"
#include <QApplication>
#include <QDeclarativeView>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qmlRegisterType<Plotline>("CustomComponents", 1, 0, "PlotLine");

    QDeclarativeView view;
    view.setSource(QUrl("./ui.qml"));
    view.setResizeMode(QDeclarativeView::SizeRootObjectToView);

    view.setGeometry(100,100, 800, 480);
    view.show();
    return a.exec();
}