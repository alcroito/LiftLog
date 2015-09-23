#ifndef SETTINGSMODEL_H
#define SETTINGSMODEL_H

#include <QObject>
#include <QAbstractItemModel>
#include <QIdentityProxyModel>
#include "settings_models/settings_pages.h"

class SettingsProxyModel : public QIdentityProxyModel {
    Q_OBJECT
    Q_ENUMS(SettingsProxyRoles)
public:
    enum SettingsProxyRoles {
        SettingsProxyPropertiesRole = Qt::UserRole + 1,
        SettingsProxyCellTypeRole,
        SettingsProxySectionRole,
    };

    explicit SettingsProxyModel(QObject* parent = 0);
    ~SettingsProxyModel();

public slots:
    QVariant data(const QModelIndex &index, int role) const;

    void init(QString pageId = SETTINGS_PAGE_INITIAL);
    void clear();
    void refresh();
    void cellClicked(int row);
    void cellSwitchValueChanged(int row, bool checked);
    void cellSliderValueChanged(int row, qreal value);
    void cellTextInputValueChanged(int row, int textInputDelta, QString value);
    void prependNewRow();

protected:
    virtual QHash<int, QByteArray> roleNames() const;

signals:
    void switchToSettingsPage(QString pageId);

private:
    QAbstractItemModel* sourceModel;
    QString currentPageId;
};

class GenericTreeNode;
class SettingsModel : public QAbstractItemModel
{
    Q_OBJECT
    Q_ENUMS(SettingsRoles)
public:
    enum SettingsRoles {
        SettingsPropertiesRole = Qt::UserRole + 1,
        SettingsCellTypeRole,
        SettingsSectionRole,
    };

    explicit SettingsModel(QObject* parent = 0);
    ~SettingsModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;

    GenericTreeNode* getModelItem(const QModelIndex &index) const;

protected:
    virtual QHash<int, QByteArray> roleNames() const;

public slots:

    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    void init(QString page = SETTINGS_PAGE_INITIAL);
    void clear();
    void refresh();
    void cellClicked(int row);
    void cellSwitchValueChanged(int row, bool checked);
    void cellSliderValueChanged(int row, qreal value);

    void prependNewRow();
signals:
    void switchToSettingsPage(QString pageId);

private:
    GenericTreeNode* root;
    QString currentPageId;
    QAbstractItemModel* innerModel = nullptr;
};

#endif // SETTINGSMODEL_H
