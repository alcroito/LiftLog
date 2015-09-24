#include "barbell_model.h"
#include "../app_state.h"
#include "../user.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

BarbellModel::BarbellModel(QObject *parent) : QAbstractListModel(parent)
{

}

int BarbellModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return barbellList.count();
}

Qt::ItemFlags BarbellModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;
    return Qt::ItemIsEditable | QAbstractItemModel::flags(index);
}

QVariant BarbellModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= barbellList.count())
            return QVariant();

    const Barbell& barbell = barbellList[index.row()];
    if (role == BarbellWeightRole) {
        User* currentUser = AppState::getInstance()->getCurrentUser();
        return barbell.getWeight(currentUser->getWeightSystem());
    }
    else if (role == BarbellMetricWeightRole) {
        return barbell.getWeight(User::Metric);
    }
    else if (role == BarbellImperialWeightRole) {
        return barbell.getWeight(User::Imperial);
    }
    else if (role == BarbellExerciseNameRole) {
        return barbell.getExerciseName();
    }

    return QVariant();
}

QHash<int, QByteArray> BarbellModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[BarbellWeightRole] = "weight";
    roles[BarbellMetricWeightRole] = "metricWeight";
    roles[BarbellImperialWeightRole] = "imperialWeight";
    roles[BarbellExerciseNameRole] = "exerciseName";
    return roles;
}

QList<Barbell> BarbellModel::getBarbellsFromDB()
{
    QList<Barbell> dbBarbellList;

    QSqlQuery query;
    // Show only exercises for currently active workout template.
    bool result = query.prepare("SELECT DISTINCT bu.id_exercise, bu.id_user, e.name, bu.weight_metric, bu.weight_imperial "
                  "FROM barbell_user bu "
                  "INNER JOIN workout_template_exercises wte ON wte.id_exercise = bu.id_exercise AND wte.id_workout_template = :id_workout_template "
                  "INNER JOIN exercise e ON bu.id_exercise = e.id_exercise "
                  "WHERE bu.id_user = :id_user "
                  "ORDER BY bu.weight_metric DESC");

    if (!result) {
        qWarning() << "Error preparing query to get user barbell list.";
        qWarning() << query.lastError();
        return dbBarbellList;
    }

    AppState* appState = AppState::getInstance();
    User* user = appState->getCurrentUser();
    query.bindValue(":id_user", user->getId());
    query.bindValue(":id_workout_template", user->getLastIdWorkoutTemplate());

    result = query.exec();
    if (!result) {
        qWarning() << "Error getting user barbell list.";
        qWarning() << query.lastError();
        return dbBarbellList;
    }

    while (query.next()) {
        Barbell barbell;
        barbell.setIdExercise(query.value("id_exercise").toInt());
        barbell.setIdUser(query.value("id_user").toInt());
        barbell.setExerciseName(query.value("name").toString());
        barbell.setMetricWeight(query.value("weight_metric").toReal());
        barbell.setImperialWeight(query.value("weight_imperial").toReal());
        dbBarbellList.append(barbell);
    }

    return dbBarbellList;
}

void BarbellModel::init()
{
    beginResetModel();
    barbellList = getBarbellsFromDB();
    endResetModel();
}

QVariant BarbellModel::getSettingsProperties(const QModelIndex &index) const
{
    QVariantMap settingsMap;
    AppState* appState = AppState::getInstance();
    User* user = appState->getCurrentUser();
    User::WeightSystem system = user->getWeightSystem();

    settingsMap.insert("cellType", "textEdit");
    settingsMap.insert("section", tr("BAR WEIGHT"));
    settingsMap.insert("label", data(index, BarbellModel::BarbellExerciseNameRole));
    settingsMap.insert("value1", data(index, BarbellModel::BarbellWeightRole));
    settingsMap.insert("displayValue1Suffix", appState->getWeightSystemSuffix(system));
    settingsMap.insert("value1ValidateAsWeight", true);
    settingsMap.insert("value1ValidatorBottomDouble", 1.0);
    settingsMap.insert("value1ValidatorTopDouble", 999.0);

    return settingsMap;
}

QVariant BarbellModel::getSettingsCellType(const QModelIndex &index) const
{
    Q_UNUSED(index;)
    return QString("textEdit");
}

QVariant BarbellModel::getSettingsSection(const QModelIndex &index) const
{
    Q_UNUSED(index;)
    return tr("BAR WEIGHT");
}

void BarbellModel::refresh()
{
    beginResetModel();
    endResetModel();
}

void BarbellModel::cellTextInputValueChanged(int row, int textInputDelta, QString value)
{
    Barbell& b = barbellList[row];
    AppState* appState = AppState::getInstance();
    User* user = appState->getCurrentUser();
    auto weightSystem = user->getWeightSystem();
    if (textInputDelta == 0) {
        auto weightValue = value.toDouble();
        b.setWeight(weightValue, weightSystem);
        auto inverseWeightSystem = appState->getInverseWeightSystem(weightSystem);
        auto inverseWeightSystemWeight = appState->truncToTwoDecimals(appState->getWeightTransformed(weightValue, weightSystem, inverseWeightSystem));
        b.setWeight(inverseWeightSystemWeight, inverseWeightSystem);
        saveUserBarbell(b);
    }
}

void BarbellModel::resetToDefaults() {
    resetUserBarbells();
}

void BarbellModel::resetUserBarbells()
{
    bool result;
    QSqlQuery query;

    query.prepare("DELETE FROM barbell_user WHERE id_user = :id_user");
    AppState* appState = AppState::getInstance();
    User* user = appState->getCurrentUser();
    query.bindValue(":id_user", user->getId());
    result = query.exec();

    if (!result) {
        qWarning() << "Error removing user barbells for reset operation.";
        qWarning() << query.lastError();
    } else {
        user->copyDefaultBarbellsIntoUserPlatesTable();
        init();
    }
}

bool BarbellModel::saveUserBarbell(Barbell& b) {
    bool result;
    QSqlQuery query;

    query.prepare("UPDATE barbell_user "
                  "SET weight_metric = :weight_metric, weight_imperial = :weight_imperial "
                  "WHERE id_exercise = :id_exercise AND id_user = :id_user"
                  );

    query.bindValue(":id_exercise", b.getIdExercise());
    query.bindValue(":id_user", b.getIdUser());
    query.bindValue(":weight_metric", b.getWeight(User::Metric));
    query.bindValue(":weight_imperial", b.getWeight(User::Imperial));
    result = query.exec();

    if (!result) {
        qWarning() << "Error saving user barbell.";
        qWarning() << query.lastError();
    }

    return result;
}
