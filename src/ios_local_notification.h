#ifndef IOS_LOCAL_NOTIFICATION
#define IOS_LOCAL_NOTIFICATION

#include <QString>
#include <QDate>
#include "local_notification_service.h"
//#include <QtGui/qpa/qplatformnativeinterface.h>


class IOSNotification : public LocalNotificationServiceImpl {
public:
    void init();
    void scheduleNotification(QString id, QDateTime date, QString title, QString text, QString actionText, qint32 badgeNumber = 0);
    void cancelAllNotifications();
    void cancelNotification(QString id);
};

#endif // IOS_LOCAL_NOTIFICATION

