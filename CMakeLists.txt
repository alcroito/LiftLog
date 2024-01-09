cmake_minimum_required(VERSION 3.16)
project(LiftLog VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui Qml Quick Sql)
if(ANDROID)
    find_package(Qt${QT_VERSION_MAJOR} OPTIONAL_COMPONENTS AndroidExtras)
endif()


if(QT_VERSION_MAJOR GREATER_EQUAL 6)
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_OSX_DEPLOYMENT_TARGET 11)
else()
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_OSX_DEPLOYMENT_TARGET 10.13)
endif()



set(sources
    src/app_state.cpp src/app_state.h
    src/application.h
    src/barbell.cpp src/barbell.h
    src/calendar_model.cpp src/calendar_model.h
    src/calendar_slider_model.cpp src/calendar_slider_model.h
    src/date_picker_model.cpp src/date_picker_model.h
    src/db_manager.cpp src/db_manager.h
    src/exercise_weight_diagram_model.cpp src/exercise_weight_diagram_model.h
    src/exercise_weight_increment.cpp src/exercise_weight_increment.h
    src/generic_tree_node.h
    src/icons.h
    src/local_notification_service.cpp src/local_notification_service.h
    src/main.cpp
    src/plate.cpp src/plate.h
    src/punits.h
    src/set_and_rep_model.cpp src/set_and_rep_model.h
    src/settings_models/barbell_model.cpp src/settings_models/barbell_model.h
    src/settings_models/exercise_sets_and_reps_model.cpp src/settings_models/exercise_sets_and_reps_model.h
    src/settings_models/main_settings_model.cpp src/settings_models/main_settings_model.h
    src/settings_models/plates_model.cpp src/settings_models/plates_model.h
    src/settings_models/settings_interface.h
    src/settings_models/settings_pages.h
    src/settings_models/settings_proxy_model.cpp src/settings_models/settings_proxy_model.h
    src/settings_models/weight_increments_model.cpp src/settings_models/weight_increments_model.h
    src/stats_graph_data.cpp src/stats_graph_data.h
    src/stats_graph_slider_model.cpp src/stats_graph_slider_model.h
    src/user.cpp src/user.h
    src/weight_string_builder.h
    src/workout_entity.cpp src/workout_entity.h
    src/workout_exercise_entity.cpp src/workout_exercise_entity.h
    src/workout_model.cpp src/workout_model.h
    src/workout_set_entity.cpp src/workout_set_entity.h
    src/workout_template_list_model.cpp src/workout_template_list_model.h
)

if(QT_VERSION_MAJOR GREATER_EQUAL 6)
    qt_standard_project_setup()

    qt_add_executable(LiftLog
        WIN32 MACOSX_BUNDLE
        MANUAL_FINALIZATION
        ${sources}
    )
else()
    set(CMAKE_AUTOMOC TRUE)

    if(ANDROID)
        add_library(LiftLog SHARED
            ${sources}
        )
    else()
        add_executable(LiftLog
            WIN32 MACOSX_BUNDLE
            ${sources}
        )
    endif()
    target_compile_features(LiftLog PRIVATE cxx_std_11)
endif()

# qt_add_qml_module(LiftLog
#     URI LiftLog
#     VERSION ${PROJECT_VERSION}
#     QML_FILES
#         LiftLog/main.qml
#         LiftLog/+android/UI.js
#         LiftLog/+ios/UI.js
#         LiftLog/+osx/UI.js
#         LiftLog/UI.js
#         LiftLog/components/SwitchStyled.qml
#         LiftLog/components/SetAndRep.qml
#         LiftLog/components/ExerciseStatsInput.qml
#         LiftLog/components/DateAndWeight.qml
#         LiftLog/components/TopNotification.qml
#         LiftLog/components/RectangleButton.qml
#         LiftLog/components/NavigationBar.qml
#         LiftLog/components/testing_ground.qml
#         LiftLog/components/TextInputStyled.qml
#         LiftLog/components/Shadow.qml
#         LiftLog/components/SwitchStyleIOS.qml
#         LiftLog/components/FinishWorkoutButton.qml
#         LiftLog/components/Separator.qml
#         LiftLog/components/SwipeArea.qml
#         LiftLog/components/CalendarNavigation.qml
#         LiftLog/components/CalendarSwipeable.qml
#         LiftLog/components/CustomStackViewDelegate.qml
#         LiftLog/utils/dummy.js
#         LiftLog/utils/Bean.qml
#         LiftLog/pages/BasicPage.qml
#         LiftLog/pages/WelcomePage.qml
#         LiftLog/pages/DashboardPage.qml
#         LiftLog/pages/WorkoutPage.qml
#         LiftLog/pages/SideWindow.qml
#         LiftLog/pages/ModalPopup.qml
#         LiftLog/pages/CalendarPage.qml
#         LiftLog/pages/BodyWeightPage.qml
#         LiftLog/extras/TumblerColumn.qml
#         LiftLog/extras/Tumbler.qml
#         LiftLog/extras/TumblerStyle.qml
#         LiftLog/extras/BaseTumblerStyle.qml
#         LiftLog/pages/BottomPopupView.qml
#         LiftLog/extras/FlatStyle.qml
#         LiftLog/components/DatePicker.qml
#         LiftLog/components/DatePickerDialog.qml
#         LiftLog/pages/ExerciseWeightPage.qml
#         LiftLog/components/ExerciseWeightDiagram.qml
#         LiftLog/pages/GraphPage.qml
#         LiftLog/components/GraphSwipeable.qml
#         LiftLog/components/Triangle.qml
#         LiftLog/components/DialogTextBox.qml
#         LiftLog/components/SectionedTableView.qml
#         LiftLog/pages/SettingsPage.qml
#         LiftLog/components/SliderStyled.qml
#         LiftLog/components/CellTypeBase.qml
#         LiftLog/components/CellTypeTextAndIcon.qml
#         LiftLog/components/CellTypeSwitch.qml
#         LiftLog/components/CellTypeSlider.qml
#         LiftLog/components/CellTypeTextEdit.qml
#         LiftLog/components/CellTypeDoubleTextEdit.qml
#         LiftLog/pages/PlatesBarbellSettingsPage.qml
#         LiftLog/pages/IncrementsSettingsPage.qml
#         LiftLog/components/TabViewStyleIOS.qml
#         LiftLog/components/CellTextInputWithSuffix.qml
#         LiftLog/components/CellTypeIncrement.qml
#         LiftLog/components/SectionedTableViewSection.qml
#     RESOURCES
#         LiftLog/qmldir
#         LiftLog/components/qmldir
#         LiftLog/utils/qmldir
#         LiftLog/pages/qmldir
#         LiftLog/extras/qmldir
#     RESOURCE_PREFIX /LiftLog
#     NO_RESOURCE_TARGET_PATH
# )

# # Avoid collision between executable name and QML module directory (QTBUG-98568).
# if(NOT WIN32 AND NOT APPLE)
#     set_property(TARGET LiftLog PROPERTY OUTPUT_NAME LiftLog_app)
# endif()

qt_add_resources(resource_sources LiftLog.qrc LiftLog/qml_resources.qrc)
target_sources(LiftLog PRIVATE ${resource_sources})


target_link_libraries(LiftLog PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Qml
    Qt${QT_VERSION_MAJOR}::Quick
    Qt${QT_VERSION_MAJOR}::Sql
)


# Resources:
set(LiftLog_resource_files
    "assets/db/db_initial_install.sql"
    "assets/db/liftlog.sqlite3"
    "assets/fonts/icomoon.ttf"
    "assets/fonts/open_sans.ttf"
    "assets/fonts/open_sans_bold.ttf"
    "assets/fonts/open_sans_light.ttf"
    "i18n/liftlog_en.qm"
    "i18n/liftlog_ru.qm"
)

# qt_add_resources(LiftLog "LiftLog"
#     PREFIX
#         "/"
#     FILES
#         ${LiftLog_resource_files}
# )
# set(qml_resources1_resource_files

# )

# qt_add_resources(LiftLog "qml_resources1"
#     PREFIX
#         "/"
#     BASE
#         "LiftLog"
#     FILES
#         ${qml_resources1_resource_files}
# )

if(APPLE AND NOT IOS)
    target_sources(LiftLog PRIVATE
        src/osx_and_ios_utils.h
        src/osx_app_delegate.mm
        src/osx_notification.h src/osx_notification.mm
    )

    target_link_libraries(LiftLog PRIVATE
        "-framework Appkit"
        "-framework Foundation"
        # gui_private
    )
endif()

if(IOS)
    target_sources(LiftLog PRIVATE
        src/ios_app_delegate.mm
        src/ios_local_notification.h src/ios_local_notification.mm
        src/osx_and_ios_utils.h
    )

    # target_link_libraries(LiftLog PRIVATE
    #     gui_private
    # )
endif()

if(ANDROID)
    target_sources(LiftLog PRIVATE
        src/android_notification.cpp src/android_notification.h
    )

    target_link_libraries(LiftLog PRIVATE
        Qt${QT_VERSION_MAJOR}::AndroidExtras
    )
endif()

# if(lupdate_only)
#     target_sources(LiftLog PUBLIC
#         -=
#         LiftLog/components/*.js LiftLog/components/*.qml
#         LiftLog/pages/*.js LiftLog/pages/*.qml
#         QT
#         androidextras
#     )
# endif()

# if(NOT lupdate_only)
#     target_sources(LiftLog PUBLIC
#         =
#     )
# endif()

# install(TARGETS LiftLog
#     BUNDLE DESTINATION .
#     RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
# )

# qt_generate_deploy_qml_app_script(
#     TARGET LiftLog
#     FILENAME_VARIABLE deploy_script
#     NO_UNSUPPORTED_PLATFORM_ERROR
#     DEPLOY_USER_QML_MODULES_ON_UNSUPPORTED_PLATFORM
#     MACOS_BUNDLE_POST_BUILD
# )
# install(SCRIPT ${deploy_script})
