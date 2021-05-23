#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>

#include <QLocale>
#include <QTranslator>

#include <gst/gst.h>

#include "player.h"

int main(int argc, char *argv[])
{
    int ret;
    gst_init(&argc, &argv);
    {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
        QQuickStyle::setStyle("Dark");

        QGuiApplication app(argc, argv);

        QTranslator translator;
        const QStringList uiLanguages = QLocale::system().uiLanguages();
        for (const QString &locale : uiLanguages) {
            const QString baseName = "deepstream_video_player_" + QLocale(locale).name();
            if (translator.load(":/i18n/" + baseName)) {
                app.installTranslator(&translator);
                break;
            }
        }
        /* the plugin must be loaded before loading the qml file to register the
         * GstGLVideoItem qml item
         */
        GstElement *sink = gst_element_factory_make ("qmlglsink", NULL);

        qmlRegisterType<Player>("Player", 1, 0, "Player");

        QQmlApplicationEngine engine;
        const QUrl url(QStringLiteral("qrc:/main.qml"));
        QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                        &app, [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        }, Qt::QueuedConnection);
        engine.load(url);
        ret = app.exec();
    }
    gst_deinit();
    return ret;
}
