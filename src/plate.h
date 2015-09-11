#ifndef PLATE
#define PLATE

#include <QObject>

class Plate {
public:
    Plate() {}
    Plate(qreal newMetricWeight) : metricWeight(newMetricWeight) {}
    Plate(qreal newMetricWeight, qreal newImperialWeight) : metricWeight(newMetricWeight), imperialWeight(newImperialWeight) {}

    qint64 getId() const;
    qint32 getCount() const;
    void setId(qint64 value);
    void setCount(qint32 value);
    void setMetricWeight(qreal value);
    void setImperialWeight(qreal value);
    qreal getWeight(int system) const;
private:
    qint64 id = 0;
    qreal metricWeight = 0;
    qreal imperialWeight = 0;
    qint32 count = 0;

};

#endif // PLATE

