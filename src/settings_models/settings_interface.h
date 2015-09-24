#ifndef SETTINGS_INTERFACE
#define SETTINGS_INTERFACE

#include <Qvariant>
#include <QModelIndex>

class SettingsInterface {
public:
    virtual ~SettingsInterface() {}
    virtual QVariant getSettingsProperties(const QModelIndex &index) const = 0;
    virtual QVariant getSettingsCellType(const QModelIndex &index) const = 0;
    virtual QVariant getSettingsSection(const QModelIndex &index) const = 0;
    virtual void refresh() = 0;
    virtual void cellClicked(int row) { Q_UNUSED(row); }
    virtual void cellSwitchValueChanged(int row, bool checked) { Q_UNUSED(row); Q_UNUSED(checked); }
    virtual void cellSliderValueChanged(int row, qreal value) { Q_UNUSED(row); Q_UNUSED(value); }
    virtual void cellTextInputValueChanged(int row, int textInputDelta, QString value) { Q_UNUSED(row); Q_UNUSED(textInputDelta); Q_UNUSED(value); }
    virtual void cellIncrementChanged(int row, bool increase) { Q_UNUSED(row); Q_UNUSED(increase); }
    virtual void prependNewRow() {}
    virtual void removeRow(int row) { Q_UNUSED(row); }
    virtual void resetToDefaults() {}

signals:
    virtual void switchToSettingsPage(QString pageId) = 0;
};

#define SettingsInterface_iid "eu.placinta.LiftLog.SettingsInterface"
Q_DECLARE_INTERFACE(SettingsInterface, SettingsInterface_iid)

#endif // SETTINGS_INTERFACE

