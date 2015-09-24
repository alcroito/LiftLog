#ifndef PLATESMODEL_H
#define PLATESMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include "../plate.h"
#include "settings_interface.h"

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
    void cellTextInputValueChanged(int row, int textInputDelta, QString value);
    void prependNewRow();
    void removeRow(int row);
    void resetToDefaults();

    qint64 getNextUserPlateId();
    void checkAndSumUpSameWeightPlates();
    void sortByWeightDesc();
public slots:
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

    QList<Plate> getPlatesFromDB();
    bool saveUserPlate(Plate &p);
    bool deleteUserPlate(Plate &p);
    void removeUserPlateDBAndCell(int row);
    void resetUserPlates();
protected:
     QHash<int, QByteArray> roleNames() const;

signals:
    void switchToSettingsPage(QString pageId);
    void willRemovePlate(int row);
    void willCheckAndSumUpSameWeightPlates();

private:
    QList<Plate> plateList;
};

#endif // PLATESMODEL_H
