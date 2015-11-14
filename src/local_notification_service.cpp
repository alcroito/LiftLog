#include "local_notification_service.h"
#include <QDebug>

#ifdef Q_OS_IOS
    #include "ios_local_notification.h"
#elif defined(Q_OS_OSX)
    #include "osx_notification.h"
#elif defined(Q_OS_ANDROID)
    #include "android_notification.h"
#endif


LocalNotificationService::LocalNotificationService(QObject *parent) : QObject(parent) {
#ifdef Q_OS_IOS
    implementation = new IOSNotification();
#elif defined(Q_OS_OSX)
    implementation = new OSXNotification();
#elif defined(Q_OS_ANDROID)
    implementation = new AndroidNotification();
#else
    implementation = new LocalNotificationServiceImpl();
#endif
    implementation->init();
    qDebug() << "Notification service created.";
}

LocalNotificationService* LocalNotificationService::getInstance()
{
    static LocalNotificationService* instance;
    if (!instance) {
         instance = new LocalNotificationService();
    }
    return instance;
}

LocalNotificationService::~LocalNotificationService()
{
    delete implementation;
    qDebug() << "Notification service destroyed.";
}

void LocalNotificationService::scheduleNotification(QString id, QDateTime date, QString title, QString text, QString actionText, qint32 badgeNumber)
{
    implementation->scheduleNotification(id, date, title, text, actionText, badgeNumber);
}

void LocalNotificationService::scheduleNotification(QString id, qint64 interval, QString title, QString text, QString actionText, qint32 badgeNumber)
{
    // @TODO Find out what kind of timezone should be used, or if UTC is ok.
    QDateTime date = QDateTime::currentDateTimeUtc().addSecs(interval);
    implementation->scheduleNotification(id, date, title, text, actionText, badgeNumber);
}

void LocalNotificationService::cancelAllNotifications()
{
    implementation->cancelAllNotifications();
}

void LocalNotificationService::cancelNotification(QString id)
{
    implementation->cancelNotification(id);
}

QDateTime LocalNotificationService::getCurrentDateTime()
{
    return QDateTime::currentDateTime();
}

LocalNotificationService* LocalNotificationService::data()
{
    return LocalNotificationService::getInstance();
}

QObject* localNotificationServiceSingletonTypeProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    LocalNotificationService* instance = LocalNotificationService::getInstance();
    return instance;
}
