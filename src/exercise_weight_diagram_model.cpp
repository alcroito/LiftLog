#include "exercise_weight_diagram_model.h"
#include "workout_exercise_entity.h"
#include "app_state.h"
#include "user.h"
#include <QDebug>

ExerciseWeightDiagramModel::ExerciseWeightDiagramModel(QObject *parent) : QAbstractListModel(parent), weight(20)
{
    // @TODO Make the available plates list configurable.
    availablePlates.append(ExercisePlate(20, 45));
    availablePlates.append(ExercisePlate(10, 25));
    availablePlates.append(ExercisePlate(5, 10));
    availablePlates.append(ExercisePlate(2.5, 5));
    availablePlates.append(ExercisePlate(1.25, 2.5));
    availablePlates.append(ExercisePlate(1, 2));
}

ExerciseWeightDiagramModel::~ExerciseWeightDiagramModel()
{

}

void ExerciseWeightDiagramModel::initPlateList(qreal newWeight) {
    weight = newWeight;

    // @TODO Fix barbell weight loading.
    WorkoutExerciseEntity* entity = new WorkoutExerciseEntity();
    qreal workingWeight = newWeight - entity->getBarbellWeight();
    delete entity;

    // Divide by two, because we only tell how much weight to put on a single side of the barbell.
    workingWeight /= 2;

    // Depending on which system we are working, we will be using different plate weight values, for more user-friendly output.
    User::WeightSystem system = AppState::getInstance()->getCurrentUser()->getWeightSystem();
    workingWeight = AppState::getInstance()->getWeightTransformed(workingWeight, User::Metric, system);

    beginResetModel();
    while (workingWeight > 0) {
        int i;
        // Go through each available plate, and see if we can subtract it's weight from the working weight.
        for  (i= 0; i < availablePlates.count(); ++i) {
            if (workingWeight >= availablePlates[i].getWeight(system)) {
                plateList.append(availablePlates[i]);
                workingWeight -= availablePlates[i].getWeight(system);
                break;
            }
        }

        // We can't subtract any more, so we're done processing the list of plates to show.
        if (i == availablePlates.count()) break;
    }
    endResetModel();
}

void ExerciseWeightDiagramModel::switchWeight(qreal newWeight) {
    if (rowCount() > 0) {
        beginRemoveRows(QModelIndex(), 0, rowCount() - 1);
        plateList.clear();
        endRemoveRows();
    }

    initPlateList(newWeight);
}

QString ExerciseWeightDiagramModel::getPlatesBreakdownText()
{
    QString text;
    qint32 plateCount = 0;
    qreal prevPlateWeight = 0;
    ExercisePlate prevPlate;
    bool prevPlateWeightSet = false;

    AppState* instance = AppState::getInstance();
    User::WeightSystem system = instance->getCurrentUser()->getWeightSystem();
    QString weightSystemSuffix = instance->getWeightSystemSuffix(system, true);

    qint32 plateListCount = plateList.count();

    for (int i = 0; i < plateListCount; ++i) {
        ExercisePlate& plate = plateList[i];
        if (plate.weight != prevPlateWeight && prevPlateWeightSet) {
            text += QString("%1 x %2%3\n").arg(plateCount).arg(prevPlate.getWeight(system)).arg(weightSystemSuffix);
            plateCount = 0;
        }
        plateCount++;
        prevPlateWeight = plate.weight;
        prevPlateWeightSet = true;
        prevPlate = plate;
    }
    if (plateCount != 0) {
        text += QString("%1 x %2%3\n").arg(plateCount).arg(prevPlate.getWeight(system)).arg(weightSystemSuffix);
        plateCount = 0;
    }

    WorkoutExerciseEntity* entity = new WorkoutExerciseEntity();
    qreal barbellWeight = entity->getBarbellWeight();
    delete entity;

    QString weightString = instance->getWeightStringBuilder(barbellWeight)->setLowerCase(true)->get();
    text += QString("1 x %1 Barbell").arg(weightString);

    return text;
}

int ExerciseWeightDiagramModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return plateList.count();
}

QVariant ExerciseWeightDiagramModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= plateList.count())
            return QVariant();

    const ExercisePlate& plate = plateList[index.row()];
    if (role == WeightRole)
        return plate.weight;
    return QVariant();
}

QHash<int, QByteArray> ExerciseWeightDiagramModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[WeightRole] = "weight";
    return roles;
}

qreal ExercisePlate::getWeight(int system)
{
    if (system == User::Metric) return weight;
    else return imperialWeight;
}
