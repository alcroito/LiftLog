#include "settings_proxy_model.h"
#include "../generic_tree_node.h"
#include "../app_state.h"
#include "../user.h"
#include "../icons.h"
#include "plates_model.h"
#include "barbell_model.h"
#include "weight_increments_model.h"
#include "main_settings_model.h"
#include "settings_interface.h"

#include <QDebug>

SettingsProxyModel::SettingsProxyModel(QObject *parent) : QIdentityProxyModel(parent)
{

}

SettingsProxyModel::~SettingsProxyModel()
{
    if (sourceModel) delete sourceModel;
}

QHash<int, QByteArray> SettingsProxyModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[SettingsProxyPropertiesRole] = "properties";
    roles[SettingsProxyCellTypeRole] = "cellType";
    roles[SettingsProxySectionRole] = "section";
    return roles;
}

QVariant SettingsProxyModel::data(const QModelIndex &index, int role) const
{
    if (role == SettingsProxyPropertiesRole) {
        SettingsInterface* i = qobject_cast<SettingsInterface*>(sourceModel);
        if (i) {
            return i->getSettingsProperties(mapToSource(index));
        }
    } else if (role == SettingsProxyCellTypeRole) {
        SettingsInterface* i = qobject_cast<SettingsInterface*>(sourceModel);
        if (i) {
            return i->getSettingsCellType(mapToSource(index));
        }
    }
    else if (role == SettingsProxySectionRole) {
        SettingsInterface* i = qobject_cast<SettingsInterface*>(sourceModel);
        if (i) {
            return i->getSettingsSection(mapToSource(index));
        }
    }

    return QVariant();
}

void SettingsProxyModel::init(QString pageId)
{
    if (pageId == SETTINGS_PAGE_INITIAL) {
        MainSettingsModel* m = new MainSettingsModel();
        m->init();
        sourceModel = m;
        setSourceModel(sourceModel);
        connect(m, SIGNAL(switchToSettingsPage(QString)), this, SIGNAL(switchToSettingsPage(QString)));
    }
    else if (pageId == SETTINGS_PAGE_WEIGHT) {
        MainSettingsModel* m = new MainSettingsModel();
        m->init(SETTINGS_PAGE_WEIGHT);
        sourceModel = m;
        setSourceModel(sourceModel);
    }
    else if (pageId == SETTINGS_PAGE_PLATES) {
        PlatesModel* m = new PlatesModel();
        m->init();
        sourceModel = m;
        setSourceModel(sourceModel);
    }
    else if (pageId == SETTINGS_PAGE_BARBELL) {
        BarbellModel* m = new BarbellModel();
        m->init();
        sourceModel = m;
        setSourceModel(sourceModel);
    } else if (pageId == SETTINGS_PAGE_INCREMENTS) {
        WeightIncrementsModel* m = new WeightIncrementsModel();
        m->init();
        sourceModel = m;
        setSourceModel(sourceModel);
    }
}

void SettingsProxyModel::refresh()
{
    SettingsInterface* i = qobject_cast<SettingsInterface*>(sourceModel);
    if (i) {
        return i->refresh();
    } else {
        qWarning() << "Couldn't refresh source model of proxy model";
    }
}

void SettingsProxyModel::clear()
{
    currentPageId = SETTINGS_PAGE_INITIAL;
    if (sourceModel) {
        setSourceModel(nullptr);
        delete sourceModel;
    }
    sourceModel = nullptr;

}

void SettingsProxyModel::cellClicked(int row)
{
    SettingsInterface* i = qobject_cast<SettingsInterface*>(sourceModel);
    if (i) {
        return i->cellClicked(row);
    } else {
        qWarning() << "Couldn't delegate cellClicked slot to source model.";
    }
}

void SettingsProxyModel::cellSwitchValueChanged(int row, bool checked)
{
    SettingsInterface* i = qobject_cast<SettingsInterface*>(sourceModel);
    if (i) {
        return i->cellSwitchValueChanged(row, checked);
    } else {
        qWarning() << "Couldn't delegate cellSwitchValueChanged slot to source model.";
    }
}

void SettingsProxyModel::cellSliderValueChanged(int row, qreal value)
{
    SettingsInterface* i = qobject_cast<SettingsInterface*>(sourceModel);
    if (i) {
        return i->cellSliderValueChanged(row, value);
    } else {
        qWarning() << "Couldn't delegate cellSliderValueChanged slot to source model.";
    }
}

void SettingsProxyModel::cellTextInputValueChanged(int row, int textInputDelta, QString value)
{
    SettingsInterface* i = qobject_cast<SettingsInterface*>(sourceModel);
    if (i) {
        return i->cellTextInputValueChanged(row, textInputDelta, value);
    } else {
        qWarning() << "Couldn't delegate cellTextInputValueChanged slot to source model.";
    }
}

void SettingsProxyModel::cellIncrementChanged(int row, bool increase)
{
    SettingsInterface* i = qobject_cast<SettingsInterface*>(sourceModel);
    if (i) {
        return i->cellIncrementChanged(row, increase);
    } else {
        qWarning() << "Couldn't delegate cellIncrementChanged slot to source model.";
    }
}

void SettingsProxyModel::prependNewRow()
{
    SettingsInterface* i = qobject_cast<SettingsInterface*>(sourceModel);
    if (i) {
        return i->prependNewRow();
    } else {
        qWarning() << "Couldn't delegate prependNewRow slot to source model.";
    }
}

void SettingsProxyModel::removeRow(int row)
{
    SettingsInterface* i = qobject_cast<SettingsInterface*>(sourceModel);
    if (i) {
        return i->removeRow(row);
    } else {
        qWarning() << "Couldn't delegate removeRow slot to source model.";
    }
}

void SettingsProxyModel::resetToDefaults()
{
    SettingsInterface* i = qobject_cast<SettingsInterface*>(sourceModel);
    if (i) {
        return i->resetToDefaults();
    } else {
        qWarning() << "Couldn't delegate resetToDefaults slot to source model.";
    }
}

void SettingsProxyModel::clearAllRows()
{
    SettingsInterface* i = qobject_cast<SettingsInterface*>(sourceModel);
    if (i) {
        return i->clearAllRows();
    } else {
        qWarning() << "Couldn't delegate clearAllRowsAndFakeFirstSectionOnly slot to source model.";
    }
}
