#include "plates_model.h"
#include "../app_state.h"
#include "../user.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

PlatesModel::PlatesModel(QObject *parent) : QAbstractListModel(parent)
{
    connect(this, &PlatesModel::willRemovePlate, this, &PlatesModel::removeUserPlateDBAndCell, Qt::QueuedConnection);
    connect(this, &PlatesModel::willCheckAndSumUpSameWeightPlates, this, &PlatesModel::checkAndSumUpSameWeightPlates, Qt::QueuedConnection);
}

int PlatesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return plateList.count();
}

Qt::ItemFlags PlatesModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;
    return Qt::ItemIsEditable | QAbstractItemModel::flags(index);
}

QVariant PlatesModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= plateList.count())
            return QVariant();

    const Plate& plate = plateList[index.row()];
    if (role == PlateWeightRole) {
        User* currentUser = AppState::getInstance()->getCurrentUser();
        return plate.getWeight(currentUser->getWeightSystem());
    }
    else if (role == PlateMetricWeightRole) {
        return plate.getWeight(User::Metric);
    }
    else if (role == PlateImperialWeightRole) {
        return plate.getWeight(User::Imperial);
    }
    else if (role == PlateCountRole) {
        return plate.getCount();
    }
    return QVariant();
}

QHash<int, QByteArray> PlatesModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[PlateWeightRole] = "weight";
    roles[PlateMetricWeightRole] = "metricWeight";
    roles[PlateImperialWeightRole] = "imperialWeight";
    roles[PlateCountRole] = "count";
    return roles;
}

QList<Plate> PlatesModel::getPlatesFromDB()
{
    QList<Plate> dbPlateList;

    QSqlQuery query;
    query.prepare("SELECT id_plate, id_user, weight_metric, weight_imperial, plate_count "
                  "FROM plate_user "
                  "WHERE id_user = :id_user "
                  "ORDER BY weight_metric DESC");

    AppState* appState = AppState::getInstance();
    User* user = appState->getCurrentUser();
    query.bindValue(":id_user", user->getId());

    bool result = query.exec();
    if (!result) {
        qWarning() << "Error getting user plate list.";
        qWarning() << query.lastError();
        return dbPlateList;
    }

    while (query.next()) {
        Plate plate;
        plate.setId(query.value("id_plate").toInt());
        plate.setUserId(query.value("id_user").toInt());
        plate.setMetricWeight(query.value("weight_metric").toReal());
        plate.setImperialWeight(query.value("weight_imperial").toReal());
        plate.setCount(query.value("plate_count").toInt());
        dbPlateList.append(plate);
    }

    return dbPlateList;
}

void PlatesModel::init()
{
    beginResetModel();
    plateList = getPlatesFromDB();
    endResetModel();
}

void PlatesModel::prependNewPlate()
{
    beginResetModel();

    Plate p(0, 0);
    p.setCount(2);

    AppState* appState = AppState::getInstance();
    User* user = appState->getCurrentUser();
    p.setUserId(user->getId());
    saveUserPlate(p);

    plateList.prepend(p);
    endResetModel();
}

QVariant PlatesModel::getSettingsProperties(const QModelIndex &index) const
{
    QVariantMap settingsMap;
    AppState* appState = AppState::getInstance();
    User* user = appState->getCurrentUser();
    User::WeightSystem system = user->getWeightSystem();

    settingsMap.insert("cellType", "doubleTextEdit");
    settingsMap.insert("section", tr("AVAILABLE PLATES"));
    settingsMap.insert("value1", data(index, PlatesModel::PlateWeightRole));
    settingsMap.insert("displayValue1Suffix", appState->getWeightSystemSuffix(system));
    settingsMap.insert("value1ValidateAsWeight", true);
    settingsMap.insert("value2", data(index, PlatesModel::PlateCountRole));
    settingsMap.insert("displayValue2Suffix", "");
    settingsMap.insert("value2ValidateAsInt", true);
    settingsMap.insert("deletable", true);
    return settingsMap;
}

QVariant PlatesModel::getSettingsCellType(const QModelIndex &index) const
{
    Q_UNUSED(index;)
    return QString("doubleTextEdit");
}

QVariant PlatesModel::getSettingsSection(const QModelIndex &index) const
{
    Q_UNUSED(index;)
    return tr("AVAILABLE PLATES");
}

void PlatesModel::refresh()
{
    beginResetModel();
    endResetModel();
}

void PlatesModel::cellTextInputValueChanged(int row, int textInputDelta, QString value)
{
    Plate& p = plateList[row];
    AppState* appState = AppState::getInstance();
    User* user = appState->getCurrentUser();
    auto weightSystem = user->getWeightSystem();
    if (textInputDelta == 0) {
        auto weightValue = value.toDouble();
        if (weightValue == 0) {
            emit willRemovePlate(row);
        } else {
            p.setWeight(weightValue, weightSystem);
            auto inverseWeightSystem = appState->getInverseWeightSystem(weightSystem);
            auto inverseWeightSystemWeight = appState->truncToTwoDecimals(appState->getWeightTransformed(weightValue, weightSystem, inverseWeightSystem));
            p.setWeight(inverseWeightSystemWeight, inverseWeightSystem);
            saveUserPlate(p);
            emit willCheckAndSumUpSameWeightPlates();
        }
    } else if (textInputDelta == 1) {
        auto countValue = value.toInt();
        if (countValue == 0) {
            // Remove plate if it has 0 as the count of plates available.
            emit willRemovePlate(row);
        } else {
            p.setCount(value.toInt());
            saveUserPlate(p);
        }

    }
}

void PlatesModel::sortByWeightDesc() {
    AppState* appState = AppState::getInstance();
    User* user = appState->getCurrentUser();
    auto weightSystem = user->getWeightSystem();
    std::sort(plateList.begin(), plateList.end(), [&weightSystem](const Plate& a, const Plate& b) {
        return a.getWeight(weightSystem) > b.getWeight(weightSystem);
    });
}

void PlatesModel::checkAndSumUpSameWeightPlates() {
    AppState* appState = AppState::getInstance();
    User* user = appState->getCurrentUser();
    auto weightSystem = user->getWeightSystem();

    auto it = plateList.begin();
    using Weight = qreal;
    using ListIndex = int;
    QHash<Weight, ListIndex> platesVisited;

    beginResetModel();
    int i = 0;
    while (it != plateList.end()) {
        auto weight = it->getWeight(weightSystem);
        // If no plate with this weight was found, stores it's list index.
        if (!platesVisited.contains(weight)) {
            platesVisited.insert(weight, i);
            ++it;
            ++i;
        } else {
            // A plate with the same weight was found.
            Plate p = *it;
            Plate& existingPlate = plateList[platesVisited[weight]];

            // Add this plate's count to the first encountered plate with the same weight.
            existingPlate.setCount(existingPlate.getCount() + p.getCount());
            saveUserPlate(existingPlate);

            // Remove this plate.
            bool result = deleteUserPlate(p);
            if (result) {
                it = plateList.erase(it);
            }
        }
    }
    sortByWeightDesc();
    endResetModel();
}

void PlatesModel::removeUserPlateDBAndCell(int row) {
    beginResetModel();
    Plate p = plateList[row];
    bool result = deleteUserPlate(p);
    if (result) {
        plateList.removeAt(row);
    }
    endResetModel();
}

void PlatesModel::resetToDefaults() {
    resetUserPlates();
}

void PlatesModel::resetUserPlates()
{
    bool result;
    QSqlQuery query;

    query.prepare("DELETE FROM plate_user WHERE id_user = :id_user");
    AppState* appState = AppState::getInstance();
    User* user = appState->getCurrentUser();
    query.bindValue(":id_user", user->getId());
    result = query.exec();

    if (!result) {
        qWarning() << "Error removing user plates for reset operation.";
        qWarning() << query.lastError();
    } else {
        user->copyDefaultPlatesIntoUserPlatesTable();
        init();
    }
}

qint64 PlatesModel::getNextUserPlateId()
{
    QSqlQuery query;
    bool result = query.exec("SELECT id_plate FROM plate_user ORDER BY id_plate DESC LIMIT 1");
    if (!result) {
        qWarning() << "Error getting next user plate id.";
        qWarning() << query.lastError();
    }

    qint64 id = 1;
    while (query.next()) {
        if (query.value(0).isValid()) {
            id = query.value(0).toInt() + 1;
        }
    }

    return id;
}

bool PlatesModel::saveUserPlate(Plate& p) {
    bool result;
    QSqlQuery query;
    if (!p.getId()) {
        p.setId(getNextUserPlateId());
        query.prepare("INSERT INTO plate_user (id_plate, id_user, weight_metric, weight_imperial, plate_count) "
                      "VALUES (:id_plate, :id_user, :weight_metric, :weight_imperial, :plate_count)");
    }
    else {
        query.prepare("UPDATE plate_user "
                      "SET weight_metric = :weight_metric, weight_imperial = :weight_imperial, plate_count = :plate_count "
                      "WHERE id_plate = :id_plate"
                      );
    }

    query.bindValue(":id_plate", p.getId());
    query.bindValue(":id_user", p.getUserId());
    query.bindValue(":weight_metric", p.getWeight(User::Metric));
    query.bindValue(":weight_imperial", p.getWeight(User::Imperial));
    query.bindValue(":plate_count", p.getCount());
    result = query.exec();

    if (!result) {
        qWarning() << "Error saving user plate.";
        qWarning() << query.lastError();
    }

    return result;
}

bool PlatesModel::deleteUserPlate(Plate& p) {
    bool result = false;
    QSqlQuery query;
    if (!p.getId()) {
        qWarning() << "No plate ID to use for deletion.";
    }
    else {
        query.prepare("DELETE FROM plate_user "
                      "WHERE id_plate = :id_plate"
                      );
        query.bindValue(":id_plate", p.getId());
        result = query.exec();

        if (!result) {
            qWarning() << "Error deleting user plate.";
            qWarning() << query.lastError();
        }
    }

    return result;
}

void PlatesModel::prependNewRow()
{
    prependNewPlate();
}

void PlatesModel::removeRow(int row)
{
    emit willRemovePlate(row);
}
