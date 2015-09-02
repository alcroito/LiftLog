#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QScreen>
#include <QDebug>
#include <QtQml>
#include <QFont>
#include <QFontDatabase>

#include "application.h"
#include "punits.h"
#include "db_manager.h"
#include "app_state.h"
#include "user.h"
#include "local_notification_service.h"
#include "workout_model.h"
#include "workout_entity.h"
#include "workout_template_list_model.h"
#include "calendar_model.h"
#include "calendar_slider_model.h"
#include "date_picker_model.h"
#include "set_and_rep_model.h"
#include "exercise_weight_diagram_model.h"
#include "stats_graph_data.h"
#include "weight_string_builder.h"
#include "stats_graph_slider_model.h"
#include "settings_model.h"

int main(int argc, char *argv[])
{
    srand(time(NULL));
//    qputenv("QML_USE_GLYPHCACHE_WORKAROUND", QByteArray("1"));
//    qputenv("QT_ANDROID_DISABLE_GLYPH_CACHE_WORKAROUND", QByteArray("1"));

    QtQuickControlsApplication app(argc, argv);

    // Set up font.
    qDebug() << QFontDatabase().addApplicationFont(":/assets/fonts/open_sans.ttf");
    qDebug() << QFontDatabase().addApplicationFont(":/assets/fonts/open_sans_bold.ttf");
    qDebug() << QFontDatabase().addApplicationFont(":/assets/fonts/open_sans_light.ttf");
    app.setFont(QFont("Open Sans"));

    // Engine.
    QQmlApplicationEngine engine;

    // Set up QML types.
    const char* uri = "LiftLog";
    qmlRegisterType<User>(uri, 1, 0, "User");
    qmlRegisterType<AppState>(uri, 1, 0, "AppState");
    qmlRegisterSingletonType<LocalNotificationService>(uri, 1, 0, "LocalNotificationService", localNotificationServiceSingletonTypeProvider);
    qmlRegisterType<WorkoutModel>(uri, 1, 0, "WorkoutModel");
    qmlRegisterType<WorkoutEntity>(uri, 1, 0, "WorkouEntity");
    qmlRegisterType<WorkoutTemplateListModel>(uri, 1, 0, "WorkoutTemplateListModel");
    qmlRegisterType<CalendarModel>(uri, 1, 0, "CalendarModel");
    qmlRegisterType<CalendarSliderModel>(uri, 1, 0, "CalendarSliderModel");
    qmlRegisterType<DatePickerModel>(uri, 1, 0, "DatePickerModel");
    qmlRegisterType<SetAndRepModel>(uri, 1, 0, "SetAndRepModel");
    qmlRegisterType<ExerciseAndSetIndexPair>(uri, 1, 0, "ExerciseSetAndPair");
    qmlRegisterType<ExerciseWeightDiagramModel>(uri, 1, 0, "ExerciseWeightDiagramModel");
    qmlRegisterType<StatsGraphData>(uri, 1, 0, "StatsGraphData");
    qmlRegisterType<StatsGraphSliderModel>(uri, 1, 0, "StatsGraphSliderModel");
    qmlRegisterType<SettingsModel>(uri, 1, 0, "SettingsModel");
    qmlRegisterUncreatableType<WeightStringBuilder>(uri, 1, 0, "WeightStringBuilder", "Not instantiable");
    qmlRegisterSingletonType<StatsGraphDataSingleton>(uri, 1, 0, "StatsGraphDataSingleton", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject* {
        Q_UNUSED(engine);
        Q_UNUSED(scriptEngine);
        auto instance = new StatsGraphDataSingleton();
        return instance;
    });
    qmlRegisterUncreatableType<PUnits>(uri, 0, 1, "PUnits", "Not instantiable");
    qmlRegisterSingletonType(QUrl("qrc:///LiftLog/extras/FlatStyle.qml"), "LiftLog.extras", 1, 0, "FlatStyle");

    // Initialize the units class.
    QQmlContext* rootContext = engine.rootContext();
    rootContext->setContextProperty("units", &PUnits::instance());

    // Create DB manager.
    QSharedPointer<DBManager> dbManager = QSharedPointer<DBManager>(new DBManager());
    rootContext->setContextProperty("dbManager", dbManager.data());

    // Prepare app state.
    QSharedPointer<AppState> appState = QSharedPointer<AppState>(new AppState());
    QScreen* screen = app.primaryScreen();
    appState->computeScaleFactors(&app, screen);
    appState->figureOutBestWindowSize(screen);
    rootContext->setContextProperty("appState", appState.data());

    // Initialize the DB manager after setting up the connections between the DB and the application state.
    // It's sad that we can't keep it in the constructor.
    QObject::connect(dbManager.data(), &DBManager::dbInitialized, appState.data(), &AppState::loadActiveUserOnDBInit);
    QObject::connect(dbManager.data(), &DBManager::dbDestroyed, appState.data(), &AppState::clearActiveUserOnDBClose);
    dbManager->init();

    // Add search path for QML modules and components.
    qDebug() << "Loading qml components.";
    engine.addImportPath("qrc:///");
    engine.load(QUrl(QStringLiteral("qrc:///LiftLog/main.qml")));    

    // Init local notification service.
    LocalNotificationService* localNotificationService = LocalNotificationService::getInstance();
    rootContext->setContextProperty("localNotificationService", localNotificationService);

//    QDateTime currentDate = QDateTime::currentDateTime();
//    QDateTime notificationDate = currentDate.addSecs(5);
//    localNotificationService->scheduleNotification("id5", notificationDate, "Awesome title2", "Hello world2", "View", 1);

    return app.exec();
}
