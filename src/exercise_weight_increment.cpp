#include "exercise_weight_increment.h"
#include "user.h"

ExerciseWeightIncrement::ExerciseWeightIncrement()
{

}

qint64 ExerciseWeightIncrement::getIdExercise() const
{
    return idExercise;
}

void ExerciseWeightIncrement::setIdExercise(const qint64 &value)
{
    idExercise = value;
}

qint64 ExerciseWeightIncrement::getIdUser() const
{
    return idUser;
}

void ExerciseWeightIncrement::setIdUser(const qint64 &value)
{
    idUser = value;
}

qreal ExerciseWeightIncrement::getMetricWeight() const
{
    return metricWeight;
}

void ExerciseWeightIncrement::setMetricWeight(const qreal &value)
{
    metricWeight = value;
}

qreal ExerciseWeightIncrement::getImperialWeight() const
{
    return imperialWeight;
}

void ExerciseWeightIncrement::setImperialWeight(const qreal &value)
{
    imperialWeight = value;
}

qreal ExerciseWeightIncrement::getWeight(int system) const
{
    if (system == User::Metric) return metricWeight;
    else return imperialWeight;
}

void ExerciseWeightIncrement::setWeight(qreal value, int system) {
    if (system == User::Metric) setMetricWeight(value);
    else setImperialWeight(value);
}

QString ExerciseWeightIncrement::getExerciseName() const
{
    return exerciseName;
}

void ExerciseWeightIncrement::setExerciseName(const QString &value)
{
    exerciseName = value;
}
