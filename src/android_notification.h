#ifndef ANDROID_NOTIFICATION_H
#define ANDROID_NOTIFICATION_H

#include <QString>
#include <QDate>
#include "local_notification_service.h"

class AndroidNotification : public LocalNotificationServiceImpl
{
public:
    void init();
    void scheduleNotification(QString id, QDateTime date, QString title, QString text, QString actionText, qint32 badgeNumber = 0);
    void cancelAllNotifications();
    void cancelNotification(QString id);
};
#endif // ANDROID_NOTIFICATION_H
