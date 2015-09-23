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
    virtual void prependNewRow() {}

signals:
    virtual void switchToSettingsPage(QString pageId) = 0;
};

#define SettingsInterface_iid "eu.placinta.LiftLog.SettingsInterface"
Q_DECLARE_INTERFACE(SettingsInterface, SettingsInterface_iid)

#endif // SETTINGS_INTERFACE
