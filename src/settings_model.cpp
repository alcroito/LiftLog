#include "settings_model.h"
#include "generic_tree_node.h"
#include "app_state.h"
#include "user.h"
#include "icons.h"
#include <QDebug>

SettingsModel::SettingsModel(QObject* parent) : QAbstractItemModel(parent), root(0), currentPageId(PAGE_INITIAL) {}

SettingsModel::~SettingsModel()
{
    clear();
}

const QString SettingsModel::PAGE_INITIAL = "initial";
const QString SettingsModel::PAGE_WEIGHT = "weight";

void SettingsModel::clear() {
    if (root) delete root;
    root = 0;
    currentPageId = PAGE_INITIAL;
}

int SettingsModel::rowCount(const QModelIndex &parent) const
{
    GenericTreeNode* parentItem;
    if (parent.column() > 0) return 0;

    if (!parent.isValid())
        parentItem = root;
    else
        parentItem = static_cast<GenericTreeNode*>(parent.internalPointer());

    if (!parentItem) return 0;
    return parentItem->childCount();
}

int SettingsModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<GenericTreeNode*>(parent.internalPointer())->columnCount();
    else
        return root->columnCount();
}

Qt::ItemFlags SettingsModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;
    return Qt::ItemIsEditable | QAbstractItemModel::flags(index);
}

QVariant SettingsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(section);
    Q_UNUSED(orientation);
    Q_UNUSED(role);
    return QVariant();
}

QModelIndex SettingsModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    GenericTreeNode* parentItem;

    if (!parent.isValid())
        parentItem = root;
    else
        parentItem = static_cast<GenericTreeNode*>(parent.internalPointer());

    GenericTreeNode* childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex SettingsModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    GenericTreeNode *childItem = static_cast<GenericTreeNode*>(index.internalPointer());
    GenericTreeNode *parentItem = childItem->parentItem();

    if (parentItem == root)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

GenericTreeNode* SettingsModel::getModelItem(const QModelIndex &index) const
{
    if (index.isValid()) {
        GenericTreeNode* item = static_cast<GenericTreeNode*>(index.internalPointer());
        if (item)
            return item;
    }
    return root;
}

QVariant SettingsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    GenericTreeNode *item = static_cast<GenericTreeNode*>(index.internalPointer());
    if (!item) {
        return QVariant();
    }

    QVariant itemData = item->data();
    switch (role) {
        case SettingsPropertiesRole:
        case Qt::DisplayRole:
            return itemData;
        case SettingsCellTypeRole:
            if (itemData.canConvert<QVariantMap>()) {
                QAssociativeIterable iterable = itemData.value<QAssociativeIterable>();
                auto it = iterable.find("cellType");
                if (it != iterable.end()) return *it;
                else return QVariant();
            }
            break;
        case SettingsSectionRole:
            if (itemData.canConvert<QVariantMap>()) {
                QAssociativeIterable iterable = itemData.value<QAssociativeIterable>();
                auto it = iterable.find("section");
                if (it != iterable.end()) return *it;
                else return QVariant();
            }
            break;
    }

    return QVariant();
}

bool SettingsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    GenericTreeNode* item = getModelItem(index);

    QVariant itemData = item->data();
    int itemDataType = itemData.userType();
    Q_UNUSED(itemDataType);

    bool result = false;

    if (role == SettingsPropertiesRole) {
        item->setData(value);
        result = true;
    }

    if (result)
        emit dataChanged(index, index);

    return result;
}

QHash<int, QByteArray> SettingsModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[SettingsPropertiesRole] = "properties";
    roles[SettingsCellTypeRole] = "cellType";
    roles[SettingsSectionRole] = "section";
    return roles;
}

void SettingsModel::getSettingsData(QString pageId)
{
    currentPageId = pageId;
    beginResetModel();
    QVariantMap settingsMap;
    root = new GenericTreeNode(QVariant::fromValue(settingsMap));
    GenericTreeNode* settingNode;
    User* currentUser = AppState::getInstance()->getCurrentUser();

    if (pageId == PAGE_INITIAL) {
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
        settingsMap.insert("nextPageId", PAGE_WEIGHT);
        settingNode = new GenericTreeNode(QVariant::fromValue(settingsMap), root);
        root->appendChild(settingNode);
        settingsMap.clear();

        settingsMap.insert("cellType", "text");
        settingsMap.insert("label", tr("Bar & Plates"));
        settingsMap.insert("section", tr("SETTINGS"));
        settingsMap.insert("icon", ICON_BARBELL);
        settingsMap.insert("showIcon", true);
        settingsMap.insert("showAccessory", true);
        settingsMap.insert("accessoryIcon", ICON_CHEVRON_RIGHT);
        settingsMap.insert("value", "");
        settingNode = new GenericTreeNode(QVariant::fromValue(settingsMap), root);
        root->appendChild(settingNode);
        settingsMap.clear();

        settingsMap.insert("id", "timerEnabled");
        settingsMap.insert("cellType", "switch");
        settingsMap.insert("label", tr("Timer"));
        settingsMap.insert("section", tr("SETTINGS"));
        settingsMap.insert("icon", ICON_TIMER);
        settingsMap.insert("showIcon", true);
        bool timerEnabled = currentUser->getTimerEnabled();
        settingsMap.insert("value", timerEnabled);
        settingNode = new GenericTreeNode(QVariant::fromValue(settingsMap), root);
        root->appendChild(settingNode);
        settingsMap.clear();

        settingsMap.insert("id", "timerSoundVolume");
        settingsMap.insert("cellType", "slider");
        settingsMap.insert("label", tr("Timer ring volume"));
        settingsMap.insert("section", tr("SETTINGS"));
        qreal timerSoundVolume = currentUser->getTimerSoundVolume();
        settingsMap.insert("value", timerSoundVolume);
        settingNode = new GenericTreeNode(QVariant::fromValue(settingsMap), root);
        root->appendChild(settingNode);
        settingsMap.clear();

        settingsMap.insert("cellType", "text");
        settingsMap.insert("label", tr("Auto-add weight"));
        settingsMap.insert("section", tr("SETTINGS"));
        settingsMap.insert("icon", ICON_SORT_UP);
        settingsMap.insert("showIcon", true);
        settingsMap.insert("showAccessory", true);
        settingsMap.insert("accessoryIcon", ICON_CHEVRON_RIGHT);
        settingsMap.insert("value", tr("On"));
        settingNode = new GenericTreeNode(QVariant::fromValue(settingsMap), root);
        root->appendChild(settingNode);
        settingsMap.clear();

        settingsMap.insert("cellType", "text");
        settingsMap.insert("label", tr("Sets & Reps"));
        settingsMap.insert("section", tr("SETTINGS"));
        settingsMap.insert("icon", ICON_SETS_AND_REPS);
        settingsMap.insert("showIcon", true);
        settingsMap.insert("showAccessory", true);
        settingsMap.insert("accessoryIcon", ICON_CHEVRON_RIGHT);
        settingsMap.insert("value", "");
        settingNode = new GenericTreeNode(QVariant::fromValue(settingsMap), root);
        root->appendChild(settingNode);
        settingsMap.clear();

        settingsMap.insert("cellType", "text");
        settingsMap.insert("label", tr("Backup"));
        settingsMap.insert("section", tr("DATA"));
        settingsMap.insert("icon", "");
        settingsMap.insert("showIcon", false);
        settingsMap.insert("showAccessory", true);
        settingsMap.insert("accessoryIcon", ICON_CHEVRON_RIGHT);
        settingsMap.insert("value", "");
        settingNode = new GenericTreeNode(QVariant::fromValue(settingsMap), root);
        root->appendChild(settingNode);
        settingsMap.clear();

        settingsMap.insert("cellType", "text");
        settingsMap.insert("label", tr("Recover"));
        settingsMap.insert("section", tr("DATA"));
        settingsMap.insert("icon", "");
        settingsMap.insert("showIcon", false);
        settingsMap.insert("showAccessory", true);
        settingsMap.insert("accessoryIcon", ICON_CHEVRON_RIGHT);
        settingsMap.insert("value", "");
        settingNode = new GenericTreeNode(QVariant::fromValue(settingsMap), root);
        root->appendChild(settingNode);
        settingsMap.clear();

        settingsMap.insert("cellType", "text");
        settingsMap.insert("label", tr("Export"));
        settingsMap.insert("section", tr("DATA"));
        settingsMap.insert("icon", "");
        settingsMap.insert("showIcon", false);
        settingsMap.insert("showAccessory", true);
        settingsMap.insert("accessoryIcon", ICON_CHEVRON_RIGHT);
        settingsMap.insert("value", "");
        settingNode = new GenericTreeNode(QVariant::fromValue(settingsMap), root);
        root->appendChild(settingNode);
        settingsMap.clear();

        settingsMap.insert("cellType", "text");
        settingsMap.insert("label", tr("Restore"));
        settingsMap.insert("section", tr("DATA"));
        settingsMap.insert("icon", "");
        settingsMap.insert("showIcon", false);
        settingsMap.insert("showAccessory", false);
        settingsMap.insert("accessoryIcon", "");
        settingsMap.insert("value", "");
        settingNode = new GenericTreeNode(QVariant::fromValue(settingsMap), root);
        root->appendChild(settingNode);
        settingsMap.clear();

        settingsMap.insert("cellType", "text");
        settingsMap.insert("label", tr("Reset"));
        settingsMap.insert("section", tr("DATA"));
        settingsMap.insert("icon", "");
        settingsMap.insert("showIcon", false);
        settingsMap.insert("showAccessory", false);
        settingsMap.insert("accessoryIcon", "");
        settingsMap.insert("value", "");
        settingNode = new GenericTreeNode(QVariant::fromValue(settingsMap), root);
        root->appendChild(settingNode);
        settingsMap.clear();
    }
    else if (pageId == PAGE_WEIGHT) {
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
        settingNode = new GenericTreeNode(QVariant::fromValue(settingsMap), root);
        root->appendChild(settingNode);
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
        settingNode = new GenericTreeNode(QVariant::fromValue(settingsMap), root);
        root->appendChild(settingNode);
        settingsMap.clear();
    }

    endResetModel();
}

void SettingsModel::cellClicked(int row)
{
    GenericTreeNode* node = root->child(row);
    QVariantMap map = node->data().value<QVariantMap>();
    if (map.contains("nextPageId")) {
        QString nextPageId = map["nextPageId"].toString();
        emit switchToSettingsPage(nextPageId);
    }

    if (map.contains("selectable") && map["selectable"].toBool() == true) {
        for (int i = 0; i < rowCount(); ++i) {
            GenericTreeNode* itNode = root->child(i);
            QVariantMap data = itNode->data().value<QVariantMap>();
            data["selected"] = row == i;

            QModelIndex modelIndex = index(i, 0);
            setData(modelIndex, data, SettingsPropertiesRole);
        }
    }

    if (currentPageId == PAGE_WEIGHT) {
        QVariantMap map2 = node->data().value<QVariantMap>();
        User::WeightSystem newWeightSystem = map2["internalValue"].value<User::WeightSystem>();
        User* currentUser = AppState::getInstance()->getCurrentUser();
        currentUser->setWeightSystem(newWeightSystem);
        currentUser->save();
    }
}

void SettingsModel::cellSwitchValueChanged(int row, bool checked)
{
    GenericTreeNode* node = root->child(row);
    QVariantMap map = node->data().value<QVariantMap>();
    if (currentPageId == PAGE_INITIAL) {
        if (map.contains("id") && map["id"] == "timerEnabled") {
            User* currentUser = AppState::getInstance()->getCurrentUser();
            currentUser->setTimerEnabled(checked);
            currentUser->save();
        }
    }
}

void SettingsModel::cellSliderValueChanged(int row, qreal value)
{
    GenericTreeNode* node = root->child(row);
    QVariantMap map = node->data().value<QVariantMap>();
    if (currentPageId == PAGE_INITIAL) {
        if (map.contains("id") && map["id"] == "timerSoundVolume") {
            User* currentUser = AppState::getInstance()->getCurrentUser();
            currentUser->setTimerSoundVolume(value);
            currentUser->save();
        }
    }
}

void SettingsModel::refresh()
{
    QString pageId = currentPageId;
    clear();
    getSettingsData(pageId);
}
