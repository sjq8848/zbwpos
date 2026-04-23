#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include "core/QtCompat.h"
#include "app/Application.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QApplication app(argc, argv);
    app.setOrganizationName("ZBW");
    app.setApplicationName("ZBW POS");

    QQuickStyle::setStyle("Material");

    QQmlApplicationEngine engine;

    Application zbwApp;
    if (!zbwApp.initialize(&engine)) {
        return -1;
    }

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
