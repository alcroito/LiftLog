#include "plate.h"
#include "user.h"

qint64 Plate::getId() const
{
    return id;
}

qint32 Plate::getCount() const
{
    return count;
}

void Plate::setId(qint64 value)
{
    id = value;
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

qreal Plate::getWeight(int system) const
{
    if (system == User::Metric) return metricWeight;
    else return imperialWeight;
}
