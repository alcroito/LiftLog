#ifndef LOCAL_NOTIFICATION_SERVICE_H
#define LOCAL_NOTIFICATION_SERVICE_H

#include <QObject>
#include <QDateTime>
#include <QQmlEngine>
#include <QDebug>

class LocalNotificationServiceImpl {
public:
    virtual void scheduleNotification(QString id, QDateTime date, QString title, QString text, QString actionText, qint32 badgeNumber = 0) {
        Q_UNUSED(id);
        Q_UNUSED(date);
        Q_UNUSED(title);
        Q_UNUSED(text);
        Q_UNUSED(actionText);
        Q_UNUSED(badgeNumber);
        qDebug() << "No local notification service implementation for this platform.";
    }

    virtual void init() {}
    virtual void destroy() {}

    virtual void cancelAllNotifications() {}
    virtual void cancelNotification(QString id) { Q_UNUSED(id); }

    virtual ~LocalNotificationServiceImpl() {}
};

class LocalNotificationService : public QObject
{
    Q_OBJECT
    explicit LocalNotificationService(QObject *parent = 0);
public:
    static LocalNotificationService* getInstance();
    ~LocalNotificationService();

    LocalNotificationService* data();

signals:
    void notificationFired(QString notificationId);

public slots:
    void scheduleNotification(QString id, QDateTime date, QString title, QString text, QString actionText, qint32 badgeNumber = 0);
    void scheduleNotification(QString id, qint64 interval, QString title, QString text, QString actionText, qint32 badgeNumber = 0);
    void cancelAllNotifications();
    void cancelNotification(QString id);

private:
    LocalNotificationServiceImpl* implementation;
};

// Declare the singleton type provider function (callback).
QObject* localNotificationServiceSingletonTypeProvider(QQmlEngine *engine, QJSEngine *scriptEngine);

#endif // LOCAL_NOTIFICATION_SERVICE_H
