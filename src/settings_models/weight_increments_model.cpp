#include "weight_increments_model.h"
#include "../app_state.h"
#include "../user.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

WeightIncrementsModel::WeightIncrementsModel(QObject *parent) : QAbstractListModel(parent)
{
}

int WeightIncrementsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return incrementList.count();
}

Qt::ItemFlags WeightIncrementsModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemFlags();
    return Qt::ItemIsEditable | QAbstractItemModel::flags(index);
}

QVariant WeightIncrementsModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= incrementList.count())
            return QVariant();

    const ExerciseWeightIncrement& increment = incrementList[index.row()];
    if (role == WeightIncrementWeightRole) {
        User* currentUser = AppState::getInstance()->getCurrentUser();
        return increment.getWeight(currentUser->getWeightSystem());
    }
    else if (role == WeightIncrementMetricWeightRole) {
        return increment.getWeight(User::Metric);
    }
    else if (role == WeightIncrementImperialWeightRole) {
        return increment.getWeight(User::Imperial);
    }
    else if (role == WeightIncrementExerciseNameRole) {
        return increment.getExerciseName();
    }

    return QVariant();
}

bool WeightIncrementsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    bool result = false;
    ExerciseWeightIncrement& i = incrementList[index.row()];
    switch (role) {
        case WeightIncrementMetricWeightRole:
            i.setMetricWeight(value.toReal());
            result = true;
            break;
        case WeightIncrementImperialWeightRole:
            i.setImperialWeight(value.toReal());
            result = true;
            break;
    }

    if (result)
        emit dataChanged(index, index);

    return result;
}

QHash<int, QByteArray> WeightIncrementsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[WeightIncrementWeightRole] = "weight";
    roles[WeightIncrementMetricWeightRole] = "metricWeight";
    roles[WeightIncrementImperialWeightRole] = "imperialWeight";
    roles[WeightIncrementExerciseNameRole] = "exerciseName";
    return roles;
}

QList<ExerciseWeightIncrement> WeightIncrementsModel::getWeightIncrementsFromDB()
{
    QList<ExerciseWeightIncrement> dbIncrementList;

    QSqlQuery query;
    // Show only exercises for currently active workout template.
    bool result = query.prepare("SELECT DISTINCT uec.id_exercise, uec.id_user, e.name, uec.weight_increment "
                  "FROM user_exercise_customisation uec "
                  "INNER JOIN workout_template_exercises wte ON wte.id_exercise = uec.id_exercise AND wte.id_workout_template = :id_workout_template "
                  "INNER JOIN exercise e ON uec.id_exercise = e.id_exercise "
                  "WHERE uec.id_user = :id_user "
                  "ORDER BY wte.delta ASC");

    if (!result) {
        qWarning() << "Error preparing query to get user weight increments list.";
        qWarning() << query.lastError();
        return dbIncrementList;
    }

    AppState* appState = AppState::getInstance();
    User* user = appState->getCurrentUser();
    query.bindValue(":id_user", user->getId());
    query.bindValue(":id_workout_template", user->getLastIdWorkoutTemplate());

    result = query.exec();
    if (!result) {
        qWarning() << "Error getting user weight increments list.";
        qWarning() << query.lastError();
        return dbIncrementList;
    }

    while (query.next()) {
        ExerciseWeightIncrement increment;
        increment.setIdExercise(query.value("id_exercise").toInt());
        increment.setIdUser(query.value("id_user").toInt());
        increment.setExerciseName(query.value("name").toString());
        increment.setMetricWeight(query.value("weight_increment").toReal());
        increment.setImperialWeight(increment.getMetricWeight() * 2);
        dbIncrementList.append(increment);
    }

    return dbIncrementList;
}

void WeightIncrementsModel::init()
{
    beginResetModel();
    incrementList = getWeightIncrementsFromDB();
    endResetModel();
}

QVariant WeightIncrementsModel::getSettingsProperties(const QModelIndex &index) const
{
    QVariantMap settingsMap;
    AppState* appState = AppState::getInstance();
    User* user = appState->getCurrentUser();
    User::WeightSystem system = user->getWeightSystem();

    settingsMap.insert("cellType", "increment");
    settingsMap.insert("section", tr("INCREMENTS"));
    settingsMap.insert("label", data(index, WeightIncrementsModel::WeightIncrementExerciseNameRole));
    settingsMap.insert("value", data(index, WeightIncrementsModel::WeightIncrementWeightRole));
    settingsMap.insert("displayValueSuffix", appState->getWeightSystemSuffix(system));

    return settingsMap;
}

QVariant WeightIncrementsModel::getSettingsCellType(const QModelIndex &index) const
{
    Q_UNUSED(index);
    auto type = QString("increment");
    return type;
}

QVariant WeightIncrementsModel::getSettingsSection(const QModelIndex &index) const
{
    Q_UNUSED(index;)
    return tr("INCREMENTS");
}

void WeightIncrementsModel::refresh()
{
    beginResetModel();
    endResetModel();
}

void WeightIncrementsModel::cellIncrementChanged(int row, bool increase)
{
    ExerciseWeightIncrement& i = incrementList[row];

    auto weightValue = i.getMetricWeight();
    if (increase) {
        if (weightValue >= 900) return;
        weightValue += 2.5;
    } else {
        if (weightValue <= 0) return;
        weightValue -= 2.5;
    }

    auto modelIndex = index(row);
    setData(modelIndex, weightValue, WeightIncrementMetricWeightRole);
    setData(modelIndex, weightValue * 2, WeightIncrementImperialWeightRole);
    saveIncrement(i);
}

void WeightIncrementsModel::resetToDefaults() {
    resetUserIncrements();
}

void WeightIncrementsModel::clearAllRows()
{
    beginResetModel();
    incrementList.clear();
    endResetModel();
}

QList<ExerciseWeightIncrement> WeightIncrementsModel::getDefaultIncrements() {
    QList<ExerciseWeightIncrement> dbIncrementList;

    QSqlQuery query;
    // Show only exercises for currently active workout template.

    bool result = query.prepare("SELECT DISTINCT e.id_exercise, e.default_weight_increment "
                  "FROM exercise e "
                  "INNER JOIN workout_template_exercises wte ON wte.id_exercise = e.id_exercise AND wte.id_workout_template = :id_workout_template "
                  "ORDER BY wte.delta ASC");

    if (!result) {
        qWarning() << "Error preparing query to get default weight increments list.";
        qWarning() << query.lastError();
        return dbIncrementList;
    }

    AppState* appState = AppState::getInstance();
    User* user = appState->getCurrentUser();
    query.bindValue(":id_workout_template", user->getLastIdWorkoutTemplate());

    result = query.exec();
    if (!result) {
        qWarning() << "Error getting default weight increments list.";
        qWarning() << query.lastError();
        return dbIncrementList;
    }

    while (query.next()) {
        ExerciseWeightIncrement increment;
        increment.setIdExercise(query.value("id_exercise").toInt());
        increment.setMetricWeight(query.value("default_weight_increment").toReal());
        increment.setImperialWeight(increment.getMetricWeight() * 2);
        dbIncrementList.append(increment);
    }

    return dbIncrementList;
}

void WeightIncrementsModel::resetUserIncrements()
{

    AppState* appState = AppState::getInstance();
    User* user = appState->getCurrentUser();

    auto defaultIncrements = getDefaultIncrements();

    for (auto& i: defaultIncrements) {
        i.setIdUser(user->getId());
        saveIncrement(i);
    }

    // Re-init from DB.
    init();
}

bool WeightIncrementsModel::saveIncrement(ExerciseWeightIncrement& i) {
    bool result;
    QSqlQuery query;

    query.prepare("UPDATE user_exercise_customisation "
                  "SET weight_increment = :weight_increment  "
                  "WHERE id_exercise = :id_exercise AND id_user = :id_user"
                  );

    query.bindValue(":id_exercise", i.getIdExercise());
    query.bindValue(":id_user", i.getIdUser());
    query.bindValue(":weight_increment", i.getWeight(User::Metric));
    result = query.exec();

    if (!result) {
        qWarning() << "Error saving user weight increment.";
        qWarning() << query.lastError();
    }

    return result;
}
