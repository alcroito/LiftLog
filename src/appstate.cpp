#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QScreen>

#include "application.h"
#include "appstate.h"
#include "user.h"
#include "punits.h"
#include "workout_model.h"

AppState::AppState(QObject *parent) : QObject(parent), current_user(new User()), currentWorkoutModel(new WorkoutModel), windowWidth(320), windowHeight(480)
{

}

void AppState::loadActiveUserOnDBInit() {
    if (isActiveUserSet()) {
        current_user->setId(getActiveUserId());
        loadCurrentUser();
    } else {
        qDebug() << "Active user not set.";
    }
}

void AppState::clearActiveUserOnDBClose() {
    // Reset the current user as if there is none set.
    current_user->clear();
}
qint32 AppState::getWindowHeight() const
{
    return windowHeight;
}

void AppState::setWindowHeight(const qint32 &value)
{
    windowHeight = value;
}

void AppState::figureOutBestWindowSize(QScreen *screen)
{
    QSize size = screen->size();
    qDebug() << "Actual size size " << size;
    qDebug() << "Available geometry " << screen->availableGeometry();
    qDebug() << "Available size" << screen->availableSize();
    qDebug() << "Available virtual size" << screen->availableVirtualSize();
    qDebug() << "Available virtual geometry" << screen->availableVirtualGeometry();
    // On OSX we stick to non-retina resolution.
#ifdef Q_OS_OSX
    setWindowWidth(320);
    setWindowHeight(480);
#else
    // For other phone devices, we use the whole space available.
    setWindowWidth(size.width());
    setWindowHeight(size.height());
#endif
}

void AppState::computeScaleFactors(QtQuickControlsApplication* app, QScreen* screen)
{
    qreal deviceDpi;

#if defined(Q_OS_WIN)
    deviceDpi = screen->logicalDotsPerInch() / app->devicePixelRatio();
#else
    deviceDpi = screen->physicalDotsPerInch() / app->devicePixelRatio();
#endif

    // Default reference dpi for medium Android devices.
    float referenceScreenDpi = 160;

#if defined(Q_OS_IOS) || defined(Q_OS_OSX)
    // Use a scale factor of 1 because Mac OS and IOS already scales implicitly for High DPI resolution screens.
    referenceScreenDpi = deviceDpi;
#endif

    float scale_factor = deviceDpi / referenceScreenDpi;
    PUnits::instance().setDpi(deviceDpi);
    PUnits::instance().setContentScaleFactor(scale_factor);
    float fontScaleFactor = scale_factor * 1.5;
    if (fontScaleFactor < 1.) fontScaleFactor = 1;
    PUnits::instance().setFontScaleFactor(fontScaleFactor);

    qDebug() << "Screen DPI" << PUnits::instance().getDpi();
    qDebug() << "Logical dots per inch" << screen->logicalDotsPerInch();
    qDebug() << "Physical dots per inch" << screen->physicalDotsPerInch();
    qDebug() << "Device pixel ratio" << app->devicePixelRatio();
    qDebug() << "Content Scale factor" << PUnits::instance().scale();
}

qint32 AppState::getWindowWidth() const
{
    return windowWidth;
}

void AppState::setWindowWidth(const qint32 &value)
{
    windowWidth = value;
}

User* AppState::getCurrentUser()
{
    return current_user.data();
}

void AppState::setCurrentUser(User* value)
{
    current_user.clear();
    current_user = QSharedPointer<User>(value);
    emit currentUserChanged();
}

WorkoutModel* AppState::getCurrentWorkoutModel()
{
    return currentWorkoutModel.data();
}

void AppState::setCurrentWorkoutModel(WorkoutModel* model)
{
    currentWorkoutModel.clear();
    currentWorkoutModel = QSharedPointer<WorkoutModel>(model);
    emit currentWorkoutModelChanged();
}

qint64 AppState::getActiveUserId()
{
    QSqlQuery query("SELECT active_id_user FROM settings");
    qint64 id = 0;
    while (query.next()) {
        id = query.value(0).toInt();
    }

    return id;
}

bool AppState::isActiveUserSet()
{
    QSqlQuery query("SELECT active_id_user FROM settings");
    bool active_user_set = false;
    while (query.next()) {
        active_user_set = !query.value(0).isNull() && query.value(0).toLongLong() != 0;
    }

    return active_user_set;
}

void AppState::saveCurrentUser() {
    qDebug() << "Saving current user.";
    bool result;
    QSqlQuery query;
    if (!current_user->getId()) {
        current_user->setId(User::getNextUserId());
        query.prepare("INSERT INTO user (id_user, name, weight_system, auto_add_weight, last_id_workout_template, last_workout_template_day) "
                      "VALUES (:id_user, :name, :weight_system, :auto_add_weight, :last_id_workout_template, :last_workout_template_day)");
        query.bindValue(":id_user", current_user->getId());
        query.bindValue(":name", current_user->getName());
        query.bindValue(":weight_system", current_user->weightSystem());
        query.bindValue(":auto_add_weight", current_user->getAutoAddWeight());
        query.bindValue(":last_id_workout_template", 0);
        query.bindValue(":last_workout_template_day", 0);
        result = query.exec();
    }
    else {
        query.prepare("UPDATE user "
                      "SET name = :name, weight_system = :weight_system, auto_add_weight = :auto_add_weight, last_id_workout_template = :last_id_workout_template, last_workout_template_day  = :last_workout_template_day "
                      "WHERE id_user = :id_user"
                      );
        query.bindValue(":id_user", current_user->getId());
        query.bindValue(":name", current_user->getName());
        query.bindValue(":weight_system", current_user->weightSystem());
        query.bindValue(":auto_add_weight", current_user->getAutoAddWeight());
        query.bindValue(":last_id_workout_template", 0);
        query.bindValue(":last_workout_template_day", 0);
        result = query.exec();
    }

    if (!result) {
        qDebug() << "Error saving current user.";
        qDebug() << query.lastError();
    }
    else {
        query.prepare("INSERT INTO settings (active_id_user) VALUES (:id_user)");
        query.bindValue(":id_user", current_user->getId());
        result = query.exec();

        if (!result) {
            qDebug() << "Error saving the current active user.";
            qDebug() << query.lastError();
        }
    }
}

void AppState::loadCurrentUser() {
    qDebug() << "Loading current user.";
    bool result;
    QSqlQuery query;

    current_user->setId(getActiveUserId());
    query.prepare("SELECT id_user, name, weight_system, auto_add_weight, last_id_workout_template, last_workout_template_day "
                  "FROM user WHERE id_user = :id_user");
    query.bindValue(":id_user", current_user->getId());
    result = query.exec();
    if (!result || !query.next()) {
        qDebug() << "Error loading current user.";
        qDebug() << query.lastError();
    }
    else {
        current_user->setName(query.value(1).toString());
        current_user->setWeightSystem((User::WeightSystem) query.value(2).toInt());
        current_user->setAutoAddWeight(query.value(3).toBool());
    }
}
