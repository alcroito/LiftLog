#ifndef PIEUTILSPLUGIN
#define PIEUTILSPLUGIN

#include <QQmlExtensionPlugin>
#include "punits.h"
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QScreen>
#include <QtQml>

class PieUtilsPlugin : public QQmlExtensionPlugin {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "pie.utils")

public:
    void registerTypes(const char* uri) {
        qmlRegisterUncreatableType<PUnits>(uri, 0, 1, "PUnits", "Not instantiable");
    }

    void initializeEngine(QQmlEngine *engine, const char *uri) {
         QQmlExtensionPlugin::initializeEngine(engine, uri);
         QQmlContext* rootContext = engine->rootContext();
         rootContext->setContextProperty("units", &PUnits::instance());
    }

};

#endif // PIEUTILSPLUGIN

