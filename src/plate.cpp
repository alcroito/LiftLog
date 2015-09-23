#include "plate.h"
#include "user.h"

qint64 Plate::getId() const
{
    return id;
}

void Plate::setId(qint64 value)
{
    id = value;
}

qint64 Plate::getUserId() const
{
    return idUser;
}

void Plate::setUserId(qint64 value)
{
    idUser = value;
}

qint32 Plate::getCount() const
{
    return count;
}

void Plate::setCount(qint32 value)
{
    count = value;
}

void Plate::setMetricWeight(qreal value)
{
    metricWeight = value;
}

void Plate::setImperialWeight(qreal value)
{
    imperialWeight = value;
}

void Plate::setWeight(qreal value, int system) {
    if (system == User::Metric) setMetricWeight(value);
    else setImperialWeight(value);
}

qreal Plate::getWeight(int system) const
{
    if (system == User::Metric) return metricWeight;
    else return imperialWeight;
}
