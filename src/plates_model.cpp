#include "plates_model.h"
#include "app_state.h"
#include "user.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

PlatesModel::PlatesModel(QObject *parent) : QAbstractListModel(parent)
{

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
    else if (role == PlateMetricWeightRole) {
        // - 2 because we don't take into account the All and Body weight entries that are in front
        return plate.getWeight(User::Imperial);
    }
    else if (role == PlateCountRole) {
        // - 2 because we don't take into account the All and Body weight entries that are in front
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
    query.prepare("SELECT id_plate, weight_metric, weight_imperial, plate_count "
                  "FROM plate_user "
                  "ORDER BY weight_metric DESC");

    bool result = query.exec();
    if (!result) {
        qDebug() << "Error getting user plate list.";
        qDebug() << query.lastError();
        return dbPlateList;
    }

    while (query.next()) {
        Plate plate;
        plate.setId(query.value("id_plate").toInt());
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

void PlatesModel::cellClicked(int row)
{

}

void PlatesModel::cellSliderValueChanged(int row, qreal value)
{

}

void PlatesModel::cellSwitchValueChanged(int row, bool checked)
{

}

void PlatesModel::prependNewRow()
{
    prependNewPlate();
}
