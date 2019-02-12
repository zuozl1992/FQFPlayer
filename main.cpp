#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include <QQuickWindow>
#include "videocontrol.h"
#include "colorquickimageprovider.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
    VideoControl *vc = new VideoControl;
    engine.rootContext()->setContextProperty("VideoControl",vc);
    engine.addImageProvider(QLatin1String("CodeImg"), vc->qfvd);
    ColorQuickImageProvider *cqip = new ColorQuickImageProvider;
    engine.addImageProvider(QLatin1String("Color"), cqip);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
