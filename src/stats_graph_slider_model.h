#ifndef STATSGRAPHSLIDERMODEL_H
#define STATSGRAPHSLIDERMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QDate>
#include "stats_graph_data.h"

class StatsGraphSliderModelEntry {

};

class StatsGraphSliderModel : public QAbstractListModel
{
    Q_OBJECT
    Q_ENUMS(StatsGraphSliderModelRoles)
public:

    enum StatsGraphSliderModelRoles {
        DataToShowRole = Qt::UserRole + 1,
        ExerciseIDRole,
        IndexRole,
        ExerciseNameRole
    };
    explicit StatsGraphSliderModel(QObject *parent = 0);

public slots:
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

protected:
     QHash<int, QByteArray> roleNames() const;

private:
    QList<StatsGraphData::StatsQueryParams> exerciseStatsParams;
};

#endif // STATSGRAPHSLIDERMODEL_H
