package eu.placinta.LiftLog;

import android.app.Notification;
import android.app.NotificationManager;
import android.content.Context;
import android.app.PendingIntent;
import android.content.Intent;
//import android.app.TaskStackBuilder;
import android.app.AlarmManager;
import java.lang.System;
import android.util.Log;
import android.os.Bundle;
import java.util.Set;
import java.util.Iterator;

public class NotificationService extends org.qtproject.qt5.android.bindings.QtActivity
{
    private static NotificationManager m_notificationManager;
    private static Notification.Builder m_builder;
    private static NotificationService m_instance;

    public NotificationService()
    {
        m_instance = this;
    }

    public static void dumpIntent(Intent i){

        Bundle bundle = i.getExtras();
        if (bundle != null) {
            Set<String> keys = bundle.keySet();
            Iterator<String> it = keys.iterator();
            Log.e("LOG_TAG", "Dumping Intent start");
            while (it.hasNext()) {
                String key = it.next();
                Log.e("LOG_TAG", "[" + key + "=" + bundle.get(key)+"]");
            }
            Log.e("LOG_TAG", "Dumping Intent end");
        }
    }

    @Override
    protected void onNewIntent (Intent intent) {
        super.onNewIntent(intent);
//        Log.d("Somestuff", "Entered new Intent");
//        Log.d("Somestuff", "Intent action" + intent.getAction());
//        Log.d("Somestuff", "Intent data" + intent.getDataString());
//        Log.d("Somestuff", "Intent flags" + intent.getFlags());
//        dumpIntent(intent);

        String maybeId = intent.getStringExtra(NotificationPublisher.NOTIFICATION_TAG);
        // Call native function to emit notificationFired signal, if the new intent
        // contains a notification id.
        if (maybeId != null) {
            notificationFired(maybeId);
        }
    }

    private static native void notificationFired(String notificationId);

    public static void init() {
        if (m_notificationManager == null) {
            // Prepare the notification manager an notification builder instances to be re-used when scheduling notifications.
            m_notificationManager = (NotificationManager) m_instance.getSystemService(Context.NOTIFICATION_SERVICE);
            m_builder = new Notification.Builder(m_instance);
            m_builder.setSmallIcon(R.drawable.icon);
            m_builder.setAutoCancel(true);
            m_builder.setDefaults(Notification.DEFAULT_SOUND | Notification.DEFAULT_VIBRATE);
        }
    }

    public static void scheduleNotification(String id, long unixTime, String title, String text, String actionText) {
        // Construct the notification.
        Notification notification = getNotification(id, title, text, actionText);

        // Create an intent to deliver the notification at a specific time.
        Intent notificationIntent = new Intent(m_instance, NotificationPublisher.class);
        notificationIntent.putExtra(NotificationPublisher.NOTIFICATION_ID, 1);
        notificationIntent.putExtra(NotificationPublisher.NOTIFICATION_TAG, id);
        notificationIntent.putExtra(NotificationPublisher.NOTIFICATION, notification);
        PendingIntent pendingIntent = PendingIntent.getBroadcast(m_instance, 0, notificationIntent, PendingIntent.FLAG_UPDATE_CURRENT);

        // Register the intent with the alarm manager.
        AlarmManager alarmManager = (AlarmManager) m_instance.getSystemService(Context.ALARM_SERVICE);
        alarmManager.setExact(AlarmManager.RTC_WAKEUP, unixTime, pendingIntent);
    }

    private static Notification getNotification(String id, String title, String text, String actionText) {
        m_builder.setContentTitle(title);
        m_builder.setContentText(text);

        // The default intent of the notification will be to open the application to the main activity (only one really).
        // It also contains the notification id.
        Intent resultIntent = new Intent(m_instance, NotificationService.class);
        resultIntent.putExtra(NotificationPublisher.NOTIFICATION_TAG, id);
        PendingIntent resultPendingIntent = PendingIntent.getActivity(m_instance, 0, resultIntent, PendingIntent.FLAG_UPDATE_CURRENT);
        m_builder.setContentIntent(resultPendingIntent);

        return m_builder.build();
    }

    public static void deliver(String id, String title, String text, String actionText)
    {
        // Deliver a notification immediately without scheduling.
        Notification notification = getNotification(id, title, text, actionText);
        m_notificationManager.notify(id, 1, notification);
    }

    public static void cancelNotification(String id) {
        m_notificationManager.cancel(id, 1);
    }

    public static void cancelAllNotifications() {
        m_notificationManager.cancelAll();
    }
}
