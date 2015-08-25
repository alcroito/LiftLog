#include <QtGui>
#include <QtQuick>
#include <UIKit/UIKit.h>
#include "ios_local_notification.h"
#include "osx_and_ios_utils.h"

void scheduleNotificationImpl(NSString* id, NSDate* date, NSString* text, NSString* actionText, qint32 badgeNumber) {
    UILocalNotification *localNotif = [[UILocalNotification alloc] init];
    if (localNotif == nil)
        return;
    localNotif.fireDate = date;
    localNotif.timeZone = [NSTimeZone defaultTimeZone];

    localNotif.alertBody = text;
    localNotif.alertAction = actionText;
    localNotif.applicationIconBadgeNumber = badgeNumber;
    localNotif.soundName = UILocalNotificationDefaultSoundName;
    localNotif.userInfo = [[NSDictionary alloc] initWithObjectsAndKeys:
        id, @"id",
        nil];

    [[UIApplication sharedApplication] scheduleLocalNotification:localNotif];
    [localNotif release];
}

void IOSNotification::init()
{

}

void IOSNotification::scheduleNotification(QString id, QDateTime date, QString title, QString text, QString actionText, qint32 badgeNumber) {
    Q_UNUSED(title);
    NSDate* iosDate = fromQDateTime(date);
    NSString* iosId = fromQString(id);
    NSString* iosText = fromQString(text);
    NSString* iosActionText = fromQString(actionText);
    scheduleNotificationImpl(iosId, iosDate, iosText, iosActionText, badgeNumber);
}

void IOSNotification::cancelAllNotifications()
{
    [[UIApplication sharedApplication] cancelAllLocalNotifications];
}

void IOSNotification::cancelNotification(QString id)
{
    NSString* iosId = fromQString(id);
    UIApplication* app = [UIApplication sharedApplication];
    NSArray* notifications = [app scheduledLocalNotifications];

    for (unsigned int i = 0; i < [notifications count]; ++i) {
        UILocalNotification* notification = [notifications objectAtIndex:i];
        NSDictionary* notificationInfo = notification.userInfo;
        NSString* notificationId = [NSString stringWithFormat:@"%@", [notificationInfo valueForKey:@"id"]];
        if ([notificationId isEqualToString:iosId]) {
            [app cancelLocalNotification:notification];
            break;
        }
    }
}

