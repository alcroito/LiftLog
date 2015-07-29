TEMPLATE = app

QT += qml quick sql

include(src/src.pri)

RESOURCES += \
    LiftLog.qrc

RESOURCES += \
    $$PWD/LiftLog/qml_resources.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH += $$PWD
QML2_IMPORT_PATH += $$PWD


# Default rules for deployment.
include(deployment.pri)

APP_QML_FILES += \
    LiftLog/main.qml \
    LiftLog/qmldir \
    LiftLog/components/qmldir \
    LiftLog/pages/qmldir \
    LiftLog/components/RectangleButton.qml \
    LiftLog/components/NavigationBar.qml \
    LiftLog/components/testing_ground.qml \
    LiftLog/components/TextInputStyled.qml \
    LiftLog/components/Shadow.qml \
    LiftLog/components/SwitchStyleIOS.qml \
    LiftLog/components/SwitchStyled.qml \
    LiftLog/pages/DashboardPage.qml \
    LiftLog/pages/WelcomePage.qml \
    LiftLog/components/SetAndRep.qml \
    LiftLog/components/ExerciseStatsInput.qml \
    LiftLog/components/TopNotification.qml \
    LiftLog/pages/WorkoutPage.qml \
    LiftLog/pages/BasicPage.qml \
    LiftLog/components/DateAndWeight.qml


FONTS += \
    icomoon.ttf

SQL_FILES += \
    assets/db/db_initial_install.sql \
    assets/db/liftlog.sqlite3

OTHER_FILES += $$APP_QML_FILES $$FONTS $$SQL_FILES

OTHER_FILES += \
    LiftLog/UI.js \
    LiftLog/+android/UI.js \
    LiftLog/+ios/UI.js \
    LiftLog/+osx/UI.js

#database_install.files = ./db_initial_install.sql
#macx: database_install.path = Contents/MacOS
#ios: database_install.path = Documents
#QMAKE_BUNDLE_DATA += database_install

macx {
    QMAKE_INFO_PLIST = mac/Info.plist
    QTPLUGIN.imageformats=-
}

ios {
    QMAKE_INFO_PLIST = ios/Info.plist
    ios_icon.files = $$files($$PWD/ios/icons/Icon*.png)
    QMAKE_BUNDLE_DATA += ios_icon
    QTPLUGIN.imageformats=-
}

android {
    QT += androidextras
    OTHER_FILES += android/AndroidManifest.xml
    QTPLUGIN.imageformats=-
}
