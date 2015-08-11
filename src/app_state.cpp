#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QScreen>
#include <QStringBuilder>

#include "application.h"
#include "app_state.h"
#include "user.h"
#include "punits.h"
#include "workout_model.h"

AppState* AppState::instance = 0;

AppState::AppState(QObject *parent) : QObject(parent), currentUser(new User()), currentWorkoutModel(new WorkoutModel), uncompletedWorkoutExists(false),
    windowWidth(320), windowHeight(480)
{
    instance = this;
}

void AppState::loadActiveUserOnDBInit() {
    if (isActiveUserSet()) {
        currentUser->setId(getActiveUserId());
        loadCurrentUser();
        recheckUncompletedWorkoutExistsValue();
    } else {
        qDebug() << "Active user not set.";
    }
}

void AppState::recheckUncompletedWorkoutExistsValue() {
    if (currentWorkoutModel->getLastNotCompletedWorkoutId(currentUser->getId()) != 0) {
        setUncompletedWorkoutExists(true);
    }
    else {
        setUncompletedWorkoutExists(false);
    }
}

void AppState::clearActiveUserOnDBClose() {
    // Reset the current user as if there is none set.
    currentUser->clear();
}

qreal AppState::getWeightTransformed(qreal weight)
{
    User::WeightSystem system = currentUser->getWeightSystem();
    if (system == User::Imperial) {
        weight *= 2;
    }

    return weight;
}

qreal AppState::getWeightTransformed(qreal weight, int from, int to)
{
    if (from == User::Imperial && to == User::Metric) {
        weight /= 2;
    }
    else if (from == User::Metric && to == User::Imperial) {
        weight *= 2;
    }

    return weight;
}

bool AppState::getUncompletedWorkoutExists() const
{
    return uncompletedWorkoutExists;
}

void AppState::setUncompletedWorkoutExists(bool value)
{
    if (value != uncompletedWorkoutExists) {
        uncompletedWorkoutExists = value;
        emit uncompletedWorkoutExistsChanged(value);
    }
}


QString AppState::getWeightString(qreal weight, bool withBodyWeight, bool withSpaceBetween)
{
    User::WeightSystem system = currentUser->getWeightSystem();
    QString weightSuffix;
    QString prefix;
    QString result;

    if (system == User::Metric) {
        weightSuffix = "KG";
    } else {
        weightSuffix = "LB";
        weight *= 2;
    }

    if (withBodyWeight) {
        prefix = "BW";
        if (weight != 0) prefix += "+";
        QTextStream(&result) << prefix;
    }

    if (weight != 0) {
        QTextStream(&result) << weight;

        if (withSpaceBetween) {
            QTextStream(&result) << " ";
        }

        QTextStream(&result) << weightSuffix;
    }

    return result;
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
    return currentUser.data();
}

void AppState::setCurrentUser(User* value)
{
    currentUser.clear();
    currentUser = QSharedPointer<User>(value);
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
    bool userSaved = currentUser->save();

    if (!userSaved) {
        qDebug() << "Error saving current user.";
    }
    else {
        QSqlQuery query;
        query.prepare("UPDATE settings SET active_id_user = :id_user");
        query.bindValue(":id_user", currentUser->getId());
        bool result = query.exec();

        if (!result) {
            qDebug() << "Error updating the active user id.";
            qDebug() << query.lastError();
        }
    }
}

void AppState::loadCurrentUser() {
    qDebug() << "Loading current user.";
    currentUser->loadById(getActiveUserId());
}
