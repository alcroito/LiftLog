#ifndef WORKOUTENTITY_H
#define WORKOUTENTITY_H

#include <QObject>
#include <QList>
#include <QDateTime>

class WorkoutExerciseEntity;

class WorkoutEntity : public QObject {
    Q_OBJECT
    Q_PROPERTY(qreal userWeight MEMBER userWeight NOTIFY userWeightChanged)
public:
    ~WorkoutEntity();
    qint64 workoutId;
    qint64 workoutTemplateId;
    qint64 day;
    QString name;
    QDateTime dateStarted;
    QDateTime dateEnded;
    QDateTime lastUpdated;
    qreal userWeight;
    bool completed;
    QList<WorkoutExerciseEntity*> exercises;

signals:
    void userWeightChanged();
};

#endif // WORKOUTENTITY_H
