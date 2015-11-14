#include "osx_notification.h"
#include "osx_and_ios_utils.h"
#include <Cocoa/Cocoa.h>
#include <Foundation/Foundation.h>
#include "Appkit/AppKit.h"

void scheduleNotificationImpl(NSString* id, NSDate* date, NSString* title, NSString* text, NSString* actionText, qint32 badgeNumber) {
    Q_UNUSED(badgeNumber);
    NSUserNotification* notification = [[NSUserNotification alloc] init];

    notification.title = title;
    notification.actionButtonTitle = actionText;
    notification.informativeText = text;
    notification.deliveryDate = date;
    notification.soundName = NSUserNotificationDefaultSoundName;
    notification.userInfo = [[NSDictionary alloc] initWithObjectsAndKeys:
        id, @"id",
        nil];

    [[NSUserNotificationCenter defaultUserNotificationCenter] scheduleNotification:notification];
    [notification release];

    //NSString* appClass = NSStringFromClass([[NSApplication sharedApplication].delegate class]);
    //NSLog(@"App class is %@", appClass);

}

void OSXNotification::scheduleNotification(QString id, QDateTime date, QString title, QString text, QString actionText, qint32 badgeNumber) {
    Q_UNUSED(title);
    NSDate* osxDate = fromQDateTime(date);
    NSString* osxId = fromQString(id);
    NSString* osxTitle = fromQString(title);
    NSString* osxText = fromQString(text);
    NSString* osxActionText = fromQString(actionText);

    scheduleNotificationImpl(osxId, osxDate, osxTitle, osxText, osxActionText, badgeNumber);
}

void OSXNotification::cancelAllNotifications()
{
    NSArray* notifications = [[NSUserNotificationCenter defaultUserNotificationCenter] scheduledNotifications];
    for (unsigned int i = 0; i < [notifications count]; ++i) {
        NSUserNotification* notification = [notifications objectAtIndex:i];
        [[NSUserNotificationCenter defaultUserNotificationCenter] removeScheduledNotification:notification];
    }
}

void OSXNotification::cancelNotification(QString id)
{
    NSString* osxId = fromQString(id);
    NSArray* notifications = [[NSUserNotificationCenter defaultUserNotificationCenter] scheduledNotifications];

    for (unsigned int i = 0; i < [notifications count]; ++i) {
        NSUserNotification* notification = [notifications objectAtIndex:i];
        NSDictionary* notificationInfo = notification.userInfo;
        NSString* notificationId = [NSString stringWithFormat:@"%@", [notificationInfo valueForKey:@"id"]];
        if ([notificationId isEqualToString:osxId]) {
            [[NSUserNotificationCenter defaultUserNotificationCenter] removeScheduledNotification:notification];
            break;
        }
    }
}

@interface UserNotificationCenterDelegate : NSObject <NSUserNotificationCenterDelegate>
- (BOOL)userNotificationCenter:(NSUserNotificationCenter *)center shouldPresentNotification:(NSUserNotification *)notification;
- (void)userNotificationCenter:(NSUserNotificationCenter *)center didDeliverNotification:(NSUserNotification *)notification;
- (void)userNotificationCenter:(NSUserNotificationCenter *)center didActivateNotification:(NSUserNotification *)notification;
@end

@implementation UserNotificationCenterDelegate
- (BOOL)userNotificationCenter:(NSUserNotificationCenter *)center shouldPresentNotification:(NSUserNotification *)notification {
    Q_UNUSED(center);
    Q_UNUSED(notification);
    return YES;
}

- (void)userNotificationCenter:(NSUserNotificationCenter *)center didDeliverNotification:(NSUserNotification *)notification {
    Q_UNUSED(center);
    if (notification) {
        NSLog(@"Recieved Notification 1 %@", notification);
    }
}

- (void)userNotificationCenter:(NSUserNotificationCenter *)center didActivateNotification:(NSUserNotification *)notification {
    Q_UNUSED(center);
    if (notification) {
        NSLog(@"Activated Notification 1 %@", notification);

        NSDictionary* notificationInfo = notification.userInfo;
        NSString* notificationId = [NSString stringWithFormat:@"%@", [notificationInfo valueForKey:@"id"]];
        QString qtId = QString::fromNSString(notificationId);

        emit LocalNotificationService::getInstance()->notificationFired(qtId);
    }
}
@end

void OSXNotification::init() {
    NSObject* center = [[NSUserNotificationCenter defaultUserNotificationCenter] delegate];
    NSString* delegateClass = NSStringFromClass([center class]);
    NSLog(@"NSUserNotificationCenterDelegate class is %@", delegateClass);

    UserNotificationCenterDelegate* delegate = [[UserNotificationCenterDelegate alloc] init];
    [[NSUserNotificationCenter defaultUserNotificationCenter] setDelegate:delegate];

    Class qtAppDelegateClass = [[[NSApplication sharedApplication] delegate] class];
    SEL newAppDelegateLaunchSelector = NSSelectorFromString(@"myAppDelegateApplicationDidFinishLaunching:");

    SwizzleReplaceMethodWithBlock(qtAppDelegateClass, @selector(applicationDidFinishLaunching:), newAppDelegateLaunchSelector, ^(NSObject* _self, NSNotification* aNotification) {
        NSLog(@"applicationDidFinishLaunching before Qt method");

        // Call the original method.
        ((void ( *)(id, SEL, NSNotification *))objc_msgSend)(_self, newAppDelegateLaunchSelector, aNotification);

        NSDictionary* notificationInfo = aNotification.userInfo;
        NSString* notificationId = [NSString stringWithFormat:@"%@", [notificationInfo valueForKey:@"id"]];
        QString qtId = QString::fromNSString(notificationId);

        emit LocalNotificationService::getInstance()->notificationFired(qtId);

        NSLog(@"The notification with which the app started is %@", aNotification);
        NSLog(@"applicationDidFinishLaunching after Qt method");
    });
}

void OSXNotification::destroy() {
    UserNotificationCenterDelegate* delegate = [[NSUserNotificationCenter defaultUserNotificationCenter] delegate];
    [delegate release];
}
