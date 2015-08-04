#include "user.h"
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>

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
    weightSystem = value;
    emit weightSystemChanged();
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

qint64 User::getNextUserId()
{
    QSqlQuery query;
    bool result = query.exec("SELECT id_user FROM user ORDER BY id_user DESC");
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

void User::clear() {
    id = 0;
    name = "";
    weightSystem = User::Metric;
    autoAddWeight = false;
    lastIdWorkoutTemplate = 0;
    nextWorkoutTemplateDay = 0;
}
