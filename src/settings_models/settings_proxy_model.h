#ifndef SETTINGSMODEL_H
#define SETTINGSMODEL_H

#include <QObject>
#include <QAbstractItemModel>
#include <QIdentityProxyModel>
#include "settings_pages.h"

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
    void cellIncrementChanged(int row, bool increase);
    void prependNewRow();    
    void removeRow(int row);
    void resetToDefaults();
protected:
    virtual QHash<int, QByteArray> roleNames() const;

signals:
    void switchToSettingsPage(QString pageId);

private:
    QAbstractItemModel* sourceModel;
    QString currentPageId;
};

#endif // SETTINGSMODEL_H
