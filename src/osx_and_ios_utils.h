#ifndef OSX_AND_IOS_UTILS
#define OSX_AND_IOS_UTILS

#include <QString>
#include <QDateTime>

#ifdef Q_OS_IOS
    #include <Foundation/Foundation.h>
#elif defined(Q_OS_OSX)
    #include <Foundation/Foundation.h>
#endif


static inline NSString* fromQString(const QString &string)
{
    const QByteArray utf8 = string.toUtf8();
    const char* cString = utf8.constData();
    return [[NSString alloc] initWithUTF8String:cString];
}

static inline NSDate* fromQDateTime(const QDateTime &dateTime)
{
    QDate datePart = dateTime.date();
    QTime timePart = dateTime.time();

    NSCalendar *calendar = [NSCalendar autoupdatingCurrentCalendar];
    NSDateComponents *dateComps = [[NSDateComponents alloc] init];

    [dateComps setDay: datePart.day()];
    [dateComps setMonth: datePart.month()];
    [dateComps setYear: datePart.year()];

    [dateComps setHour: timePart.hour()];
    [dateComps setMinute: timePart.minute()];
    [dateComps setSecond: timePart.second()];

    NSDate* date = [calendar dateFromComponents:dateComps];
    [dateComps release];
    return date;
}

#if TARGET_OS_IPHONE
    #import <objc/runtime.h>
    #import <objc/message.h>
#else
    #import <objc/objc-class.h>
#endif

BOOL SwizzleReplaceMethodWithBlock(Class c, SEL origSEL, SEL newSEL, id block) {
    if ([c respondsToSelector:newSEL]) return YES; // Selector already implemented, skip

    Method origMethod = class_getInstanceMethod(c, origSEL);

    // Add the new method.
    IMP impl = imp_implementationWithBlock(block);
    if (!class_addMethod(c, newSEL, impl, method_getTypeEncoding(origMethod))) {
        NSLog(@"Failed to add method: %@ on %@", NSStringFromSelector(newSEL), c);
        return NO;
    }else {
        Method newMethod = class_getInstanceMethod(c, newSEL);

        // If original doesn't implement the method we want to swizzle, create it.
        if (class_addMethod(c, origSEL, method_getImplementation(newMethod), method_getTypeEncoding(origMethod))) {
            class_replaceMethod(c, newSEL, method_getImplementation(origMethod), method_getTypeEncoding(newMethod));
        }else {
            method_exchangeImplementations(origMethod, newMethod);
        }
    }
    return YES;
}

#endif // OSX_AND_IOS_UTILS

