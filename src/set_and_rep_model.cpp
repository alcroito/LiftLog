#include "set_and_rep_model.h"
#include "workout_set_entity.h"
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>

SetAndRepModel::SetAndRepModel(QObject *parent) : QAbstractListModel(parent)
{

}

SetAndRepModel::~SetAndRepModel()
{

}

void SetAndRepModel::init() {
    beginResetModel();
    setAndRepList = getDBSetAndRepList();
    endResetModel();
}

qint32 SetAndRepModel::initWithSetAndRepIdAndReturnSetIndex(qint64 id) {
    beginResetModel();
    setAndRepList = getDBSetAndRepList(id);
    endResetModel();
    for (int i = 0; i < setAndRepList.count(); ++i) {
        if (setAndRepList[i].getId() == id) return i;
    }

    return 0;
}

int SetAndRepModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return setAndRepList.count();
}

QVariant SetAndRepModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= setAndRepList.count())
            return QVariant();

    const SetAndRep& setAndRep = setAndRepList[index.row()];
    if (role == TextRole)
        return setAndRep.getText();
    else if (role == StateRole)
        return setAndRep.getState();
    else if (role == IdRole)
        return setAndRep.getId();
    else if (role == SetCountRole)
        return setAndRep.getSetCount();
    else if (role == RepCountRole)
        return setAndRep.getRepCount();
    return QVariant();
}

SetAndRep SetAndRepModel::getSetAndRepById(qint64 id)
{
    SetAndRep set;
    QSqlQuery query;
    query.prepare("SELECT id_set_and_rep, set_count, consistent_rep_count AS rep_count, delta "
                  "FROM set_and_rep "
                  "WHERE id_set_and_rep = :id");
    query.bindValue(":id", id);

    bool result = query.exec();
    if (!result) {
        qDebug() << "Error getting set and rep data by id.";
        qDebug() << query.lastError();
        return set;
    }

    while (query.next()) {
        qint64 id = query.value(0).toInt();
        qint32 setCount = query.value(1).toInt();
        qint32 repCount = query.value(2).toInt();
        qint32 delta = query.value(3).toInt();
        set.id = id;
        set.setCount = setCount;
        set.repCount = repCount;
        set.state = WorkoutSetEntity::STANDARD_STATE;
        set.delta = delta;
    }

    return set;
}

QHash<int, QByteArray> SetAndRepModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[TextRole] = "text";
    roles[StateRole] = "state";
    roles[IdRole] = "setAndRepId";
    roles[SetCountRole] = "setCount";
    roles[RepCountRole] = "repCount";
    return roles;
}

QList<SetAndRep> SetAndRepModel::getDBSetAndRepList(qint64 initId) {
    QList<SetAndRep> setAndRepList;

    QSqlQuery query;
    query.prepare("SELECT id_set_and_rep, set_count, consistent_rep_count AS rep_count, delta "
                  "FROM set_and_rep "
                  "ORDER BY delta");

    bool result = query.exec();
    if (!result) {
        qDebug() << "Error getting set and rep list.";
        qDebug() << query.lastError();
        return setAndRepList;
    }

    while (query.next()) {
        SetAndRep setAndRep;
        qint64 id = query.value(0).toInt();
        qint32 setCount = query.value(1).toInt();
        qint32 repCount = query.value(2).toInt();
        qint32 delta = query.value(3).toInt();
        setAndRep.id = id;
        setAndRep.setCount = setCount;
        setAndRep.repCount = repCount;
        if (initId == id) {
            setAndRep.state = WorkoutSetEntity::ACTIVE_STATE;
        } else {
            setAndRep.state = WorkoutSetEntity::STANDARD_STATE;
        }

        setAndRep.delta = delta;
        setAndRepList.append(setAndRep);
    }

    return setAndRepList;
}

bool SetAndRepModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    bool result = false;
    SetAndRep& setAndRep = setAndRepList[index.row()];
    switch (role) {
        case StateRole:
            setAndRep.state = value.toString();
            result = true;
            break;
    }

    if (result)
        emit dataChanged(index, index);

    return result;
}

void SetAndRepModel::resetStates() {
    for (int i = 0; i < setAndRepList.count(); ++i) {
        setData(index(i), WorkoutSetEntity::STANDARD_STATE, StateRole);
    }
}
