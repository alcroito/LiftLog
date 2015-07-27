#ifndef OSX_NOTIFICATION_H
#define OSX_NOTIFICATION_H

#include <QString>
#include <QDate>
#include "local_notification_service.h"

class OSXNotification : public LocalNotificationServiceImpl
{
public:
    void init();
    void destroy();
    void scheduleNotification(QString id, QDateTime date, QString title, QString text, QString actionText, qint32 badgeNumber = 0);
    void cancelAllNotifications();
    void cancelNotification(QString id);
};

#endif // OSX_NOTIFICATION_H
