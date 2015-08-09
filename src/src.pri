SOURCES += \
    $$PWD/main.cpp \
    $$PWD/user.cpp \
    $$PWD/local_notification_service.cpp \
    $$PWD/workout_model.cpp \
    $$PWD/workout_entity.cpp \
    $$PWD/workout_exercise_entity.cpp \
    $$PWD/workout_set_entity.cpp \
    $$PWD/workout_tree_node.cpp \
    $$PWD/db_manager.cpp \
    $$PWD/app_state.cpp \
    $$PWD/workout_template_list_model.cpp \
    $$PWD/calendar_model.cpp \
    $$PWD/calendar_slider_model.cpp \
    $$PWD/date_picker_model.cpp

HEADERS += \
#    $$PWD/pieutilsplugin.h \
    $$PWD/punits.h \
    $$PWD/user.h \
    $$PWD/local_notification_service.h \
    $$PWD/application.h \
    $$PWD/workout_model.h \
    $$PWD/workout_entity.h \
    $$PWD/workout_set_entity.h \
    $$PWD/workout_exercise_entity.h \
    $$PWD/workout_tree_node.h \
    $$PWD/app_state.h \
    $$PWD/db_manager.h \
    $$PWD/workout_template_list_model.h \
    $$PWD/calendar_model.h \
    $$PWD/calendar_slider_model.h \
    $$PWD/date_picker_model.h

ios {
    HEADERS += \
        $$PWD/ios_local_notification.h \
        $$PWD/osx_and_ios_utils.h

    OBJECTIVE_SOURCES += \
        $$PWD/ios_app_delegate.mm \
        $$PWD/ios_local_notification.mm

    QT += gui_private
}

macx {
    LIBS += -framework Foundation -framework Appkit
    INCLUDEPATH += /System/Library/Frameworks/Foundation.framework/Versions/C/Headers

#    LIBS += -L/Users/placinta/Dev/Qt/5.4/clang_64/plugins/platforms -lqcocoa
#    QMAKE_LFLAGS += -whyload -all_load -undefined dynamic_lookup
#    QMAKE_LFLAGS += -all_load -undefined dynamic_lookup

    HEADERS += \
        $$PWD/osx_notification.h \
        $$PWD/osx_and_ios_utils.h

    OBJECTIVE_SOURCES += \
        $$PWD/osx_notification.mm \
        $$PWD/osx_app_delegate.mm


    OBJECTIVE_SOURCES +=

    QT += gui_private
}

android {
    ANDROID_PACKAGE_SOURCE_DIR = $$PWD/../android

    HEADERS += \
        $$PWD/android_notification.h

    SOURCES += \
        $$PWD/android_notification.cpp

    OTHER_FILES += \
        $$PWD/../android/src/eu/placinta/LiftLog/NotificationService.java \
        $$PWD/../android/src/eu/placinta/LiftLog/NotificationPublisher.java
}
