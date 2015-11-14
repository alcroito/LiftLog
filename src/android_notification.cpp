#include <QtAndroidExtras/QAndroidJniObject>
#include <QtAndroidExtras/QAndroidJniEnvironment>
#include "android_notification.h"
#include <QDebug>
#include "application.h"

const char * serviceClass = "eu/placinta/LiftLog/NotificationService";

void printAnyException() {
    QAndroidJniEnvironment env;
    if (env->ExceptionCheck()) {
        // Handle exception here.
        qDebug() << "Java Exception occurred";
        env->ExceptionDescribe();
        env->ExceptionClear();
    }
}

static void notificationFired(JNIEnv *, jclass /*clazz*/, jstring notificationId)
{
    QAndroidJniObject jString(notificationId);
    QString qtId = jString.toString();
    emit LocalNotificationService::getInstance()->notificationFired(qtId);
}


void AndroidNotification::init() {
    qDebug() << "Initializing Android Notification Service.";
    QAndroidJniObject::callStaticMethod<void>(serviceClass, "init", "()V");
    printAnyException();
}

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void*) {
    QCoreApplication* appInstance = QtQuickControlsApplication::instance();
    Q_UNUSED(appInstance);
    Q_UNUSED(vm);

    // Registering natives should always be done in the JNI_OnLoad function.
    // It can be defined once for each .so file (I can't remember where I read this).
    qDebug() << "Registering native methods for Android Notification Service.";
    JNINativeMethod methods[] = {
        {"notificationFired", "(Ljava/lang/String;)V", (void *)notificationFired}
    };


    QAndroidJniEnvironment env;

    jclass objectClass = env->FindClass(serviceClass);
    jint result = env->RegisterNatives(objectClass,
                         methods,
                         sizeof(methods) / sizeof(methods[0]));
    if (result < 0) {
        qCritical() << "Error occurred registering native methods.";
        return JNI_FALSE;
    }
    printAnyException();

    return JNI_VERSION_1_6;
}

void AndroidNotification::scheduleNotification(QString id, QDateTime date, QString title, QString text, QString actionText, qint32 badgeNumber)
{
    Q_UNUSED(badgeNumber);
    qint64 dateInMilis = static_cast<qint64>(date.toTime_t()) * 1000;
    QAndroidJniObject jId = QAndroidJniObject::fromString(id);
    QAndroidJniObject jTitle = QAndroidJniObject::fromString(title);
    QAndroidJniObject jText = QAndroidJniObject::fromString(text);
    QAndroidJniObject jActionText = QAndroidJniObject::fromString(actionText);
    jlong jDate = dateInMilis;

    qDebug() << "Before scheduling notification on Android";
    QAndroidJniObject::callStaticMethod<void>(serviceClass,
                                        "scheduleNotification",
                                        "(Ljava/lang/String;JLjava/lang/String;Ljava/lang/String;Ljava/lang/String;)V",
                                        jId.object<jstring>(), jDate, jTitle.object<jstring>(), jText.object<jstring>(), jActionText.object<jstring>());

    printAnyException();

    qDebug() << "After scheduling notification on Android";
}

void AndroidNotification::cancelAllNotifications()
{
    QAndroidJniObject::callStaticMethod<void>(serviceClass, "cancelAllNotifications", "()V");
    printAnyException();
}

void AndroidNotification::cancelNotification(QString id)
{
    QAndroidJniObject jId = QAndroidJniObject::fromString(id);
    QAndroidJniObject::callStaticMethod<void>(serviceClass, "cancelNotification", "(Ljava/lang/String;)V", jId.object<jstring>());
    printAnyException();
}
