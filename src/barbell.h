#ifndef BARBELL_H
#define BARBELL_H

#include <QObject>

class Barbell
{
public:
    Barbell();
    Barbell(qreal metricWeight_, qreal imperialWeight_)
        : metricWeight(metricWeight_), imperialWeight(imperialWeight_) {}
    Barbell(const QString& name, qreal metricWeight_, qreal imperialWeight_)
        : exerciseName(name), metricWeight(metricWeight_), imperialWeight(imperialWeight_) {}

    qint64 getIdExercise() const;
    void setIdExercise(const qint64 &value);
    qint64 getIdUser() const;
    void setIdUser(const qint64 &value);
    qreal getMetricWeight() const;
    void setMetricWeight(const qreal &value);
    qreal getImperialWeight() const;
    void setImperialWeight(const qreal &value);
    qreal getWeight(int system) const;
    void setWeight(qreal value, int system);
    QString getExerciseName() const;
    void setExerciseName(const QString &value);

private:
    qint64 idExercise = 0;
    qint64 idUser = 0;
    QString exerciseName = "";
    qreal metricWeight = 0;
    qreal imperialWeight = 0;
};

#endif // BARBELL_H
