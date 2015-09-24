#include "barbell.h"
#include "user.h"

Barbell::Barbell()
{
}

qint64 Barbell::getIdExercise() const
{
    return idExercise;
}

void Barbell::setIdExercise(const qint64 &value)
{
    idExercise = value;
}

qint64 Barbell::getIdUser() const
{
    return idUser;
}

void Barbell::setIdUser(const qint64 &value)
{
    idUser = value;
}

qreal Barbell::getMetricWeight() const
{
    return metricWeight;
}

void Barbell::setMetricWeight(const qreal &value)
{
    metricWeight = value;
}

qreal Barbell::getImperialWeight() const
{
    return imperialWeight;
}

void Barbell::setImperialWeight(const qreal &value)
{
    imperialWeight = value;
}

qreal Barbell::getWeight(int system) const
{
    if (system == User::Metric) return metricWeight;
    else return imperialWeight;
}

void Barbell::setWeight(qreal value, int system) {
    if (system == User::Metric) setMetricWeight(value);
    else setImperialWeight(value);
}

QString Barbell::getExerciseName() const
{
    return exerciseName;
}

void Barbell::setExerciseName(const QString &value)
{
    exerciseName = value;
}
