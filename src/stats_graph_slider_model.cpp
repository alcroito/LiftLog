#include "stats_graph_slider_model.h"

StatsGraphSliderModel::StatsGraphSliderModel(QObject *parent) : QAbstractListModel(parent)
{
    exerciseStatsParams = StatsGraphData::getListOfExerciseQueryParamsThatHaveEnoughPoints();
}

int StatsGraphSliderModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return exerciseStatsParams.count();
}

QVariant StatsGraphSliderModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= exerciseStatsParams.count())
            return QVariant();

    const StatsGraphData::StatsQueryParams params = exerciseStatsParams[index.row()];
    if (role == DataToShowRole) {
        return params.dataToShow;
    }
    else if (role == ExerciseIDRole) {
        return params.idExerciseToShow;
    }
    else if (role == IndexRole) {
        // - 2 because we don't take into account the All and Body weight entries that are in front
        return index.row() - 2;
    }
    else if (role == ExerciseNameRole) {
        return params.nameOfExerciseToShow;
    }
    return QVariant();
}

QHash<int, QByteArray> StatsGraphSliderModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[DataToShowRole] = "dataToShow";
    roles[ExerciseIDRole] = "idExerciseToShow";
    roles[IndexRole] = "listIndex";
    roles[ExerciseNameRole] = "exerciseNameToShow";
    return roles;
}

