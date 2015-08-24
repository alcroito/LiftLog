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
    LiftLog/components/RectangleButton.qml \
    LiftLog/components/NavigationBar.qml \
    LiftLog/components/testing_ground.qml \
    LiftLog/components/TextInputStyled.qml \
    LiftLog/components/Shadow.qml \
    LiftLog/components/SwitchStyleIOS.qml \
    LiftLog/components/SwitchStyled.qml \
    LiftLog/components/SetAndRep.qml \
    LiftLog/components/ExerciseStatsInput.qml \
    LiftLog/components/TopNotification.qml \
    LiftLog/components/DateAndWeight.qml \
    LiftLog/components/Separator.qml \
    LiftLog/components/SwipeArea.qml \
    LiftLog/components/CalendarNavigation.qml \
    LiftLog/components/CalendarSwipeable.qml \
    LiftLog/components/CustomStackViewDelegate.qml\
    LiftLog/components/DatePicker.qml\
    LiftLog/components/DatePickerDialog.qml\
    LiftLog/components/ExerciseWeightDiagram.qml\
    LiftLog/components/GraphSwipeable.qml\
    LiftLog/components/Triangle.qml\
    LiftLog/components/DialogTextBox.qml\
    LiftLog/pages/qmldir \
    LiftLog/pages/DashboardPage.qml \
    LiftLog/pages/WelcomePage.qml \
    LiftLog/pages/WorkoutPage.qml \
    LiftLog/pages/BasicPage.qml \
    LiftLog/pages/SideWindow.qml \
    LiftLog/pages/ModalPopup.qml \
    LiftLog/pages/CalendarPage.qml \
    LiftLog/pages/BodyWeightPage.qml \
    LiftLog/pages/BottomPopupView.qml \
    LiftLog/pages/ExerciseWeightPage.qml \
    LiftLog/pages/GraphPage.qml \
    LiftLog/extras/qmldir \
    LiftLog/extras/Tumbler.qml \
    LiftLog/extras/TumblerColumn.qml \
    LiftLog/extras/TumblerStyle.qml \
    LiftLog/extras/BaseTumblerStyle.qml\
    LiftLog/extras/FlatStyle.qml



FONTS += \
    icomoon.ttf
    opensans.ttf

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

mac {
    CONFIG += c++14
    QMAKE_CXXFLAGS += -stdlib=libc++
}

macx {
    QMAKE_INFO_PLIST = mac/Info.plist
    QTPLUGIN.imageformats=-
    ICON = mac/icon.icns
}

ios {
    QMAKE_INFO_PLIST = ios/Info.plist
    ios_resources.files += $$files($$PWD/ios/icons/Icon*.png)
    ios_resources.files += $$files($$PWD/ios/launch/LaunchImage*.png)
    QMAKE_BUNDLE_DATA += ios_resources

    QTPLUGIN.imageformats=-
}

android {
    QMAKE_CXXFLAGS += -std=c++1y
    QT += androidextras
    OTHER_FILES += android/AndroidManifest.xml
    QTPLUGIN.imageformats=-
}
