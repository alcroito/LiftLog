#import "UIKit/UIKit.h"
#include <QtCore>
#include "local_notification_service.h"

@interface QIOSApplicationDelegate
@end

@interface QIOSApplicationDelegate(AppDelegate)
@end

@implementation QIOSApplicationDelegate (AppDelegate)

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    Q_UNUSED(application);

    NSLog(@"didFinishLaunchingWithOptions: %@", [launchOptions description]);

    UILocalNotification *localNotif =
        [launchOptions objectForKey:UIApplicationLaunchOptionsLocalNotificationKey];
    if (localNotif) {
        NSLog(@"Recieved Notification 1 %@",localNotif);

        NSDictionary* notificationInfo = localNotif.userInfo;
        NSString* notificationId = [NSString stringWithFormat:@"%@", [notificationInfo valueForKey:@"id"]];
        QString qtId = QString::fromNSString(notificationId);

        emit LocalNotificationService::getInstance()->notificationFired(qtId);
        application.applicationIconBadgeNumber = 0;
    }

    // Register notifications for IOS8+.
    if ([application respondsToSelector:@selector(registerUserNotificationSettings:)])
    {
        [application registerUserNotificationSettings:[UIUserNotificationSettings settingsForTypes:UIUserNotificationTypeAlert|UIUserNotificationTypeBadge|UIUserNotificationTypeSound categories:nil]];
        [application registerForRemoteNotifications];
    } else {
        // IOS 7 or less.
        [application registerForRemoteNotificationTypes:UIRemoteNotificationTypeSound | UIRemoteNotificationTypeAlert | UIRemoteNotificationTypeBadge];
    }

    return YES;
}

- (void)application:(UIApplication *)app didReceiveLocalNotification:(UILocalNotification *)notification {
    Q_UNUSED(app);

    // Handle the notificaton when the app is running
    NSLog(@"Recieved Notification 2 %@", notification);
    LocalNotificationService* instance;
    instance = LocalNotificationService::getInstance();

    NSDictionary* notificationInfo = notification.userInfo;
    NSString* notificationId = [NSString stringWithFormat:@"%@", [notificationInfo valueForKey:@"id"]];
    QString qtId = QString::fromNSString(notificationId);

    emit LocalNotificationService::getInstance()->notificationFired(qtId);

    app.applicationIconBadgeNumber = 0;
}

@end
