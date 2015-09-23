#include "user.h"
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>
#include "workout_entity.h"

User::User(QObject *parent) : QObject(parent), id(0), weightSystem(User::Metric), autoAddWeight(true), lastIdWorkoutTemplate(0), nextWorkoutTemplateDay(0)
{

}
QString User::getName() const
{
    return name;
}

void User::setName(const QString &value)
{
    name = value;
    emit nameChanged();
}

User::WeightSystem User::getWeightSystem() const
{
    return weightSystem;
}

void User::setWeightSystem(const User::WeightSystem value)
{
    if (value != weightSystem) {
        weightSystem = value;
        emit weightSystemChanged(value);
    }
}

bool User::getAutoAddWeight() const
{
    return autoAddWeight;
}

void User::setAutoAddWeight(bool value)
{
    autoAddWeight = value;
    emit autoAddWeightChanged();
}

qint64 User::getId() const
{
    return id;
}

void User::setId(const qint64 &value)
{
    id = value;
    emit idChanged();
}

qint64 User::getLastIdWorkoutTemplate() const
{
    return lastIdWorkoutTemplate;
}

void User::setLastIdWorkoutTemplate(const qint64 &value)
{
    lastIdWorkoutTemplate = value;
    emit lastIdWorkoutTemplateChanged();
}

qint64 User::getNextWorkoutTemplateDay() const
{
    return nextWorkoutTemplateDay;
}

void User::setNextWorkoutTemplateDay(const qint64 &value)
{
    nextWorkoutTemplateDay = value;
    emit nextWorkoutTemplateDayChanged();
}

qreal User::getUserWeight()
{
    qreal weight = WorkoutEntity::initialUserWeight;

    QSqlQuery query;
    query.prepare("SELECT user_weight FROM user_workout WHERE id_user = :id_user ORDER BY date_started DESC LIMIT 1");
    query.bindValue(":id_user", getId());
    bool result = query.exec();
    if (!result) {
        qDebug() << "Error getting user weight.";
        qDebug() << query.lastError();
    }

    while (query.next()) {
        if (!query.isNull(0)) {
            weight = query.value(0).toReal();
        }
    }

    return weight;
}

qint64 User::getNextUserId()
{
    QSqlQuery query;
    bool result = query.exec("SELECT id_user FROM user ORDER BY id_user DESC LIMIT 1");
    if (!result) {
        qDebug() << "Error getting next user id.";
        qDebug() << query.lastError();
    }

    qint64 id = 1;
    while (query.next()) {
        if (query.value(0).isValid()) {
            id = query.value(0).toInt() + 1;
        }
    }

    return id;
}

bool User::loadById(qint64 idUser)
{
    QSqlQuery query;
    query.prepare("SELECT id_user, name, weight_system, auto_add_weight, timer_enabled, timer_sound_volume, last_id_workout_template, last_workout_template_day "
                  "FROM user WHERE id_user = :id_user");
    query.bindValue(":id_user", idUser);
    bool result = query.exec();
    bool userLoaded = result && query.next();
    if (userLoaded) {
        setId(idUser);
        setName(query.value("name").toString());
        setWeightSystem((User::WeightSystem) query.value("weight_system").toInt());
        setAutoAddWeight(query.value("auto_add_weight").toBool());
        setTimerEnabled(query.value("timer_enabled").toBool());
        setTimerSoundVolume(query.value("timer_sound_volume").toReal());
        setLastIdWorkoutTemplate(query.value("last_id_workout_template").toInt());
        setNextWorkoutTemplateDay(query.value("last_workout_template_day").toInt());
    }
    else {
        qDebug() << "Error loading user by id.";
        qDebug() << query.lastError();
    }

    return userLoaded;
}

bool User::save()
{
    bool result;
    QSqlQuery query;
    bool newUser = true;
    if (!getId()) {
        setId(getNextUserId());
        query.prepare("INSERT INTO user (id_user, name, weight_system, auto_add_weight, timer_enabled, timer_sound_volume, last_id_workout_template, last_workout_template_day) "
                      "VALUES (:id_user, :name, :weight_system, :auto_add_weight, :timer_enabled, :timer_sound_volume, :last_id_workout_template, :last_workout_template_day)");
    }
    else {
        newUser = false;
        query.prepare("UPDATE user "
                      "SET name = :name, weight_system = :weight_system, auto_add_weight = :auto_add_weight, "
                      "timer_enabled = :timer_enabled, timer_sound_volume = :timer_sound_volume, "
                      "last_id_workout_template = :last_id_workout_template, last_workout_template_day = :last_workout_template_day "
                      "WHERE id_user = :id_user"
                      );
    }

    query.bindValue(":id_user", getId());
    query.bindValue(":name", getName());
    query.bindValue(":weight_system", getWeightSystem());
    query.bindValue(":auto_add_weight", getAutoAddWeight());
    query.bindValue(":timer_enabled", getTimerEnabled());
    query.bindValue(":timer_sound_volume", getTimerSoundVolume());
    query.bindValue(":last_id_workout_template", getLastIdWorkoutTemplate());
    query.bindValue(":last_workout_template_day", getNextWorkoutTemplateDay());
    result = query.exec();

    if (!result) {
        qDebug() << "Error saving user.";
        qDebug() << query.lastError();
    } else {
        if (newUser) {
            copyDefaultPlatesIntoUserPlatesTable();
        }
    }

    return result;
}

bool User::copyDefaultPlatesIntoUserPlatesTable() {
    QSqlQuery query;
    bool result;
    result = query.prepare("INSERT INTO plate_user (id_user, weight_metric, weight_imperial, plate_count) "
                  "SELECT :id_user, weight_metric, weight_imperial, plate_count "
                  "FROM plate_default "
                  "ORDER BY id_plate ");
    if (!result) {
        qWarning() << "Error preparing query to copy default plates to user plates table.";
        qWarning() << query.lastError();
        return false;
    }
    query.bindValue(":id_user", getId());
    result = query.exec();
    if (!result) {
        qWarning() << "Error copying default plates to user plates table.";
        qWarning() << query.lastError();
        return false;
    }

    return true;
}

void User::clear() {
    id = 0;
    name = "";
    weightSystem = User::Metric;
    autoAddWeight = false;
    lastIdWorkoutTemplate = 0;
    nextWorkoutTemplateDay = 0;
}

bool User::getTimerEnabled() const
{
    return timerEnabled;
}

void User::setTimerEnabled(bool value)
{
    if (timerEnabled != value) {
        timerEnabled = value;
        emit timerEnabledChanged(value);
    }
}

qreal User::getTimerSoundVolume() const
{
    return timerSoundVolume;
}

void User::setTimerSoundVolume(const qreal &value)
{
    if (timerSoundVolume != value) {
        timerSoundVolume = value;
        emit timerSoundVolumeChanged(value);
    }
}
