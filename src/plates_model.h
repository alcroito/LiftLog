#ifndef PLATESMODEL_H
#define PLATESMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include "plate.h"
#include "settings_models/settings_interface.h"

class PlatesModel : public QAbstractListModel, public SettingsInterface
{
    Q_OBJECT
    Q_ENUMS(PlatesModelRoles)
    Q_INTERFACES(SettingsInterface)
public:
    enum PlatesModelRoles {
        PlateWeightRole = Qt::UserRole + 1,
        PlateMetricWeightRole,
        PlateImperialWeightRole,
        PlateCountRole
    };

    explicit PlatesModel(QObject *parent = 0);

    void init();
    void prependNewPlate();

    QVariant getSettingsProperties(const QModelIndex &index) const;
    QVariant getSettingsCellType(const QModelIndex &index) const;
    QVariant getSettingsSection(const QModelIndex &index) const;
    void refresh();
    void cellClicked(int row);
    void cellSliderValueChanged(int row, qreal value);
    void cellSwitchValueChanged(int row, bool checked);
    void prependNewRow();

public slots:
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

    QList<Plate> getPlatesFromDB();

protected:
     QHash<int, QByteArray> roleNames() const;

signals:
    void switchToSettingsPage(QString pageId);

private:
    QList<Plate> plateList;
};

#endif // PLATESMODEL_H