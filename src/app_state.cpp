#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QScreen>
#include <QStringBuilder>
#include <QQmlEngine>
#include <QEvent>
#include <QInputMethodQueryEvent>
#include <cmath>

#include "application.h"
#include "app_state.h"
#include "user.h"
#include "punits.h"
#include "workout_model.h"
#include "weight_string_builder.h"

#ifdef Q_OS_IOS
//#include <UIKit/UIKit.h>
#endif

AppState* AppState::instance = 0;

const qreal oneKgToPounds = 2.2046223;

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

WeightStringBuilder* AppState::getWeightStringBuilder(qreal weight)
{
    auto builder = WeightStringBuilder::create(weight);
    QQmlEngine::setObjectOwnership(builder, QQmlEngine::CppOwnership);
    return builder;
}

qreal AppState::getWeightTransformed(qreal weight, int from, int to)
{
    if (from == User::Metric && to == User::Metric) {
        // When converting from and to metric, we also round, because if there was a conversion cycle some time (Metric -> Imperial -> Metric)
        // the metric value can contain lots of decimal numbers, which we don't want.
        weight = neatRoundForMetric(weight);
    }
    else if (from == User::Imperial && to == User::Metric) {
        weight /= oneKgToPounds;
    }
    else if (from == User::Metric && to == User::Imperial) {
        // When converting from metric to imperial, we neatly round the value, so it doesn't have lots of decimal numbers.
        weight = neatRoundForImperial(weight * oneKgToPounds);
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

qreal AppState::neatRoundForImperial(qreal imperialWeight) {
    qreal remainder = std::fmod(imperialWeight, 10);
    qreal quotient = imperialWeight - remainder;
    bool negative = false;
    qreal delta = 0;
    if (remainder < 0) { remainder = std::abs(remainder); negative = true; }

    if (remainder >= 0 && remainder < 1.25) delta = 0;
    else if (remainder >= 1.25 && remainder < 3.75) delta += 2.5;
    else if (remainder >= 3.75 && remainder < 6.25) delta += 5;
    else if (remainder >= 6.25 && remainder < 8.75) delta += 7.5;
    else if (remainder >= 8.75 && remainder < 10.0) delta += 10;

    if (negative) quotient -= delta;
    else quotient += delta;

    return quotient;
}

qreal AppState::neatRoundForMetric(qreal metricWeight) {
    qreal remainder = std::fmod(metricWeight, 10);
    qreal quotient = metricWeight - remainder;
    bool negative = false;
    qreal delta = 0;
    if (remainder < 0) { remainder = std::abs(remainder); negative = true; }

    if (remainder >= 0 && remainder < 0.625) delta = 0;
    else if (remainder >= 0.625 && remainder < 1.875) delta += 1.25;
    else if (remainder >= 1.875 && remainder < 3.125) delta += 2.5;
    else if (remainder >= 3.125 && remainder < 4.375) delta += 3.75;
    else if (remainder >= 4.375 && remainder < 5.625) delta += 5;
    else if (remainder >= 5.625 && remainder < 6.875) delta += 6.25;
    else if (remainder >= 6.875 && remainder < 8.125) delta += 7.5;
    else if (remainder >= 8.125 && remainder < 9.375) delta += 8.75;
    else if (remainder >= 9.375 && remainder < 10.0) delta += 10;

    if (negative) quotient -= delta;
    else quotient += delta;

    return quotient;
}

qreal AppState::neatRoundForSystem(qreal weight, int system) {
    if (system == User::Metric) return neatRoundForMetric(weight);
    else return neatRoundForImperial(weight);
}

qreal AppState::truncToTwoDecimals(qreal value) {
    return trunc(100 * value) / 100;
}

QString AppState::getWeightSystemSuffix(User::WeightSystem system, bool lowercase) {
    QString weightSuffix;

    if (system == User::Metric) {
        weightSuffix = "KG";
    } else {
        weightSuffix = "LB";
    }

    if (lowercase) weightSuffix = weightSuffix.toLower();

    return weightSuffix;
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


QString WeightStringBuilder::build()
{
    User::WeightSystem system = AppState::getInstance()->getCurrentUser()->getWeightSystem();
    QString weightSuffix;
    QString prefix;
    QString result;

    if (system == User::Metric) {
        weightSuffix = "KG";
        if (neat)
            weight = AppState::neatRoundForMetric(weight);
    } else {
        weightSuffix = "LB";
        if (neat)
            weight = AppState::neatRoundForImperial(weight * oneKgToPounds);
        else weight *= oneKgToPounds;
    }

    if (withBodyWeight) {
        prefix = "BW";
        if (weight > 0) prefix += "+";;
        QTextStream(&result) << prefix;
    }

    if ((withBodyWeight && weight != 0) || !withBodyWeight) {
        QTextStream(&result) << weight;

        if (withSpaceBetween) {
            QTextStream(&result) << " ";
        }

        QTextStream(&result) << weightSuffix;
    }

    if (lowerCase) result = result.toLower();

    return result;
}

bool AppEventFilter::eventFilter(QObject *obj, QEvent *event)
{
    static bool here = false;
    if (event->type() == QEvent::InputMethodQuery) {
        QInputMethodQueryEvent* imEvent = static_cast<QInputMethodQueryEvent *>(event);
        QVariantMap platformData = imEvent->value(Qt::ImPlatformData).toMap();
#ifdef Q_OS_IOS
        platformData.insert("returnKeyType", 4);
        imEvent->setValue(Qt::ImPlatformData, platformData);
        qDebug() << "queries" << imEvent->queries() << "platformData" << platformData;
        auto im = QGuiApplication::inputMethod();
        if (!here) {
            here = true;
            im->update(imEvent->queries() | Qt::ImQueryAll);
            here = false;
        }
        else {
            int a = 1;
        }
#endif
        return QObject::eventFilter(obj, event);
    } else {
        // standard event processing
        return QObject::eventFilter(obj, event);
    }
}
