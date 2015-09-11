#ifndef PLATESMODEL_H
#define PLATESMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include "plate.h"

class PlatesModel : public QAbstractListModel
{
    Q_OBJECT
    Q_ENUMS(PlatesModelRoles)
public:

    enum PlatesModelRoles {
        PlateWeightRole = Qt::UserRole + 1,
        PlateMetricWeightRole,
        PlateImperialWeightRole,
        PlateCountRole
    };
    explicit PlatesModel(QObject *parent = 0);

public slots:
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

    QList<Plate> getPlatesFromDB();
    void init();
    void prependNewPlate();
    void refresh();
protected:
     QHash<int, QByteArray> roleNames() const;

private:
    QList<Plate> plateList;
};

#endif // PLATESMODEL_H
