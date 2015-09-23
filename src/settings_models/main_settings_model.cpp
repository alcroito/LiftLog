#include "main_settings_model.h"
#include "../user.h"
#include "../app_state.h"
#include "../icons.h"
#include <QDebug>

MainSettingsModel::MainSettingsModel(QObject *parent) : QAbstractListModel(parent) {}

int MainSettingsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return settingsList.count();
}

Qt::ItemFlags MainSettingsModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;
    return Qt::ItemIsEditable | QAbstractItemModel::flags(index);
}

QVariant MainSettingsModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= settingsList.count())
            return QVariant();

    int row = index.row();
    const QVariant& cellSetting = settingsList[row];

    switch (role) {
        case MainSettingsPropertiesRole:
        case Qt::DisplayRole:
            return cellSetting;
        case MainSettingsCellTypeRole:
            if (cellSetting.canConvert<QVariantMap>()) {
                QAssociativeIterable iterable = cellSetting.value<QAssociativeIterable>();
                auto it = iterable.find("cellType");
                if (it != iterable.end()) return *it;
                else return QVariant();
            }
            break;
        case MainSettingsSectionRole:
            if (cellSetting.canConvert<QVariantMap>()) {
                QAssociativeIterable iterable = cellSetting.value<QAssociativeIterable>();
                auto it = iterable.find("section");
                if (it != iterable.end()) return *it;
                else return QVariant();
            }
            break;
    }
    return QVariant();
}

QHash<int, QByteArray> MainSettingsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[MainSettingsPropertiesRole] = "properties";
    roles[MainSettingsCellTypeRole] = "cellType";
    roles[MainSettingsSectionRole] = "section";
    return roles;
}

QVariant MainSettingsModel::getSettingsProperties(const QModelIndex &index) const
{
    return data(index, MainSettingsPropertiesRole);
}

QVariant MainSettingsModel::getSettingsCellType(const QModelIndex &index) const
{
    return data(index, MainSettingsCellTypeRole);
}

QVariant MainSettingsModel::getSettingsSection(const QModelIndex &index) const
{
    return data(index, MainSettingsSectionRole);
}

void MainSettingsModel::clear()
{
    settingsList.clear();
}

void MainSettingsModel::refresh() {
    QString pageId = currentPageId;
    clear();
    init(pageId);
}

void MainSettingsModel::init(QString pageId) {
    beginResetModel();
    currentPageId = pageId;
    User* currentUser = AppState::getInstance()->getCurrentUser();
    QVariantMap settingsMap;

    if (pageId == SETTINGS_PAGE_INITIAL) {
        settingsMap.insert("cellType", "text");
        settingsMap.insert("label", tr("Weight System"));
        settingsMap.insert("section", tr("SETTINGS"));
        settingsMap.insert("icon", ICON_WEIGHT);
        settingsMap.insert("showIcon", true);
        settingsMap.insert("showAccessory", true);
        settingsMap.insert("accessoryIcon", ICON_CHEVRON_RIGHT);
        QString weightSystemValue = tr("Metric");
        if (currentUser->getWeightSystem() == User::Imperial)
            weightSystemValue = tr("Imperial");
        settingsMap.insert("value", weightSystemValue);
        settingsMap.insert("nextPageId", SETTINGS_PAGE_WEIGHT);
        settingsList.append(settingsMap);
        settingsMap.clear();

        settingsMap.insert("cellType", "text");
        settingsMap.insert("label", tr("Bar & Plates"));
        settingsMap.insert("section", tr("SETTINGS"));
        settingsMap.insert("icon", ICON_BARBELL);
        settingsMap.insert("showIcon", true);
        settingsMap.insert("showAccessory", true);
        settingsMap.insert("accessoryIcon", ICON_CHEVRON_RIGHT);
        settingsMap.insert("value", "");
        settingsMap.insert("nextPageId", SETTINGS_PAGE_PLATES);
        settingsList.append(settingsMap);
        settingsMap.clear();

        settingsMap.insert("id", "timerEnabled");
        settingsMap.insert("cellType", "switch");
        settingsMap.insert("label", tr("Timer"));
        settingsMap.insert("section", tr("SETTINGS"));
        settingsMap.insert("icon", ICON_TIMER);
        settingsMap.insert("showIcon", true);
        bool timerEnabled = currentUser->getTimerEnabled();
        settingsMap.insert("value", timerEnabled);
        settingsList.append(settingsMap);
        settingsMap.clear();

        settingsMap.insert("id", "timerSoundVolume");
        settingsMap.insert("cellType", "slider");
        settingsMap.insert("label", tr("Timer ring volume"));
        settingsMap.insert("section", tr("SETTINGS"));
        qreal timerSoundVolume = currentUser->getTimerSoundVolume();
        settingsMap.insert("value", timerSoundVolume);
        settingsList.append(settingsMap);
        settingsMap.clear();

        settingsMap.insert("cellType", "text");
        settingsMap.insert("label", tr("Auto-add weight"));
        settingsMap.insert("section", tr("SETTINGS"));
        settingsMap.insert("icon", ICON_SORT_UP);
        settingsMap.insert("showIcon", true);
        settingsMap.insert("showAccessory", true);
        settingsMap.insert("accessoryIcon", ICON_CHEVRON_RIGHT);
        settingsMap.insert("value", tr("On"));
        settingsList.append(settingsMap);
        settingsMap.clear();

        settingsMap.insert("cellType", "text");
        settingsMap.insert("label", tr("Sets & Reps"));
        settingsMap.insert("section", tr("SETTINGS"));
        settingsMap.insert("icon", ICON_SETS_AND_REPS);
        settingsMap.insert("showIcon", true);
        settingsMap.insert("showAccessory", true);
        settingsMap.insert("accessoryIcon", ICON_CHEVRON_RIGHT);
        settingsMap.insert("value", "");
        settingsList.append(settingsMap);
        settingsMap.clear();

        settingsMap.insert("cellType", "text");
        settingsMap.insert("label", tr("Backup"));
        settingsMap.insert("section", tr("DATA"));
        settingsMap.insert("icon", "");
        settingsMap.insert("showIcon", false);
        settingsMap.insert("showAccessory", true);
        settingsMap.insert("accessoryIcon", ICON_CHEVRON_RIGHT);
        settingsMap.insert("value", "");
        settingsList.append(settingsMap);
        settingsMap.clear();

        settingsMap.insert("cellType", "text");
        settingsMap.insert("label", tr("Recover"));
        settingsMap.insert("section", tr("DATA"));
        settingsMap.insert("icon", "");
        settingsMap.insert("showIcon", false);
        settingsMap.insert("showAccessory", true);
        settingsMap.insert("accessoryIcon", ICON_CHEVRON_RIGHT);
        settingsMap.insert("value", "");
        settingsList.append(settingsMap);
        settingsMap.clear();

        settingsMap.insert("cellType", "text");
        settingsMap.insert("label", tr("Export"));
        settingsMap.insert("section", tr("DATA"));
        settingsMap.insert("icon", "");
        settingsMap.insert("showIcon", false);
        settingsMap.insert("showAccessory", true);
        settingsMap.insert("accessoryIcon", ICON_CHEVRON_RIGHT);
        settingsMap.insert("value", "");
        settingsList.append(settingsMap);
        settingsMap.clear();

        settingsMap.insert("cellType", "text");
        settingsMap.insert("label", tr("Restore"));
        settingsMap.insert("section", tr("DATA"));
        settingsMap.insert("icon", "");
        settingsMap.insert("showIcon", false);
        settingsMap.insert("showAccessory", false);
        settingsMap.insert("accessoryIcon", "");
        settingsMap.insert("value", "");
        settingsList.append(settingsMap);
        settingsMap.clear();

        settingsMap.insert("cellType", "text");
        settingsMap.insert("label", tr("Reset"));
        settingsMap.insert("section", tr("DATA"));
        settingsMap.insert("icon", "");
        settingsMap.insert("showIcon", false);
        settingsMap.insert("showAccessory", false);
        settingsMap.insert("accessoryIcon", "");
        settingsMap.insert("value", "");
        settingsList.append(settingsMap);
        settingsMap.clear();
    }
    else if (pageId == SETTINGS_PAGE_WEIGHT) {
        bool metricActive = currentUser->getWeightSystem() == User::Metric;
        bool imperialActive = currentUser->getWeightSystem() == User::Imperial;
        settingsMap.insert("cellType", "text");
        settingsMap.insert("label", tr("Metric"));
        settingsMap.insert("section", tr("WEIGHT SYSTEM"));
        settingsMap.insert("icon", "");
        settingsMap.insert("showIcon", false);
        settingsMap.insert("accessoryIcon", ICON_CHECK);
        settingsMap.insert("value", "");
        settingsMap.insert("internalValue", QVariant::fromValue(User::Metric));
        settingsMap.insert("selectable", true);
        settingsMap.insert("selected", metricActive);
        settingsList.append(settingsMap);
        settingsMap.clear();

        settingsMap.insert("cellType", "text");
        settingsMap.insert("label", tr("Imperial"));
        settingsMap.insert("section", tr("WEIGHT SYSTEM"));
        settingsMap.insert("icon", "");
        settingsMap.insert("showIcon", false);
        settingsMap.insert("accessoryIcon", ICON_CHECK);
        settingsMap.insert("value", "");
        settingsMap.insert("internalValue", QVariant::fromValue(User::Imperial));
        settingsMap.insert("selectable", true);
        settingsMap.insert("selected", imperialActive);
        settingsList.append(settingsMap);
        settingsMap.clear();
    }
    endResetModel();
}

bool MainSettingsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    bool result = false;

    if (role == MainSettingsPropertiesRole) {
        settingsList[index.row()] = value.toMap();
        result = true;
    }

    if (result)
        emit dataChanged(index, index);

    return result;
}

void MainSettingsModel::cellClicked(int row)
{
    QVariantMap map = settingsList[row];
    if (map.contains("nextPageId")) {
        QString nextPageId = map["nextPageId"].toString();
        emit switchToSettingsPage(nextPageId);
    }

    if (map.contains("selectable") && map["selectable"].toBool() == true) {
        for (int i = 0; i < rowCount(); ++i) {
            QVariantMap data = settingsList[i];
            data["selected"] = row == i;

            QModelIndex modelIndex = index(i, 0);
            setData(modelIndex, data, MainSettingsPropertiesRole);
        }
    }

    if (currentPageId == SETTINGS_PAGE_WEIGHT) {
        QVariantMap map2 = settingsList[row];
        User::WeightSystem newWeightSystem = map2["internalValue"].value<User::WeightSystem>();
        User* currentUser = AppState::getInstance()->getCurrentUser();
        currentUser->setWeightSystem(newWeightSystem);
        currentUser->save();
    }
}

void MainSettingsModel::cellSwitchValueChanged(int row, bool checked)
{
    QVariantMap map = settingsList[row];
    if (currentPageId == SETTINGS_PAGE_INITIAL) {
        if (map.contains("id") && map["id"] == "timerEnabled") {
            User* currentUser = AppState::getInstance()->getCurrentUser();
            currentUser->setTimerEnabled(checked);
            currentUser->save();
        }
    }
}

void MainSettingsModel::cellSliderValueChanged(int row, qreal value)
{
    QVariantMap map = settingsList[row];
    if (currentPageId == SETTINGS_PAGE_INITIAL) {
        if (map.contains("id") && map["id"] == "timerSoundVolume") {
            User* currentUser = AppState::getInstance()->getCurrentUser();
            currentUser->setTimerSoundVolume(value);
            currentUser->save();
        }
    }
}

