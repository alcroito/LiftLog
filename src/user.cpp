#include "user.h"
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>

User::User(QObject *parent) : QObject(parent), id(0), weight_system(User::Metric), auto_add_weight(true), last_id_workout_template(0), last_workout_template_day(0)
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
User::WeightSystem User::weightSystem() const
{
    return weight_system;
}

void User::setWeightSystem(const User::WeightSystem value)
{
    weight_system = value;
    emit weightSystemChanged();
}
bool User::getAutoAddWeight() const
{
    return auto_add_weight;
}

void User::setAutoAddWeight(bool value)
{
    auto_add_weight = value;
    emit autoAddWeightChanged();
}

qint64 User::getId() const
{
    return id;
}

void User::setId(const qint64 &value)
{
    id = value;
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
    weight_system = User::Metric;
    auto_add_weight = false;
    last_id_workout_template = 0;
    last_workout_template_day = 0;
}
