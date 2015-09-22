#ifndef MAINSETTINGSMODEL_H
#define MAINSETTINGSMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include "settings_pages.h"
#include "settings_interface.h"

class MainSettingsModel : public QAbstractListModel, public SettingsInterface
{
    Q_OBJECT
    Q_ENUMS(MainSettingsRoles)
    Q_INTERFACES(SettingsInterface)
public:
    enum MainSettingsRoles {
        MainSettingsPropertiesRole = Qt::UserRole + 1,
        MainSettingsCellTypeRole,
        MainSettingsSectionRole,
    };

    explicit MainSettingsModel(QObject *parent = 0);

    QVariant getSettingsProperties(const QModelIndex &index) const;
    QVariant getSettingsCellType(const QModelIndex &index) const;
    QVariant getSettingsSection(const QModelIndex &index) const;
    void refresh();
    void clear();
    void cellClicked(int row);
    void cellSliderValueChanged(int row, qreal value);
    void cellSwitchValueChanged(int row, bool checked);

public slots:
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    Qt::ItemFlags flags(const QModelIndex &index) const;

    void init(QString pageId = SETTINGS_PAGE_INITIAL);

protected:
     QHash<int, QByteArray> roleNames() const;

signals:
     void switchToSettingsPage(QString pageId);

private:
    QList<QVariantMap> settingsList;
    QString currentPageId;
};

#endif // MAINSETTINGSMODEL_H
