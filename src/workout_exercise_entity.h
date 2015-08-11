#ifndef WORKOUTEXERCISEENTITY_H
#define WORKOUTEXERCISEENTITY_H

#include <QObject>
#include <QStringList>

class WorkoutSetEntity;

class WorkoutExerciseEntity : public QObject {
    Q_OBJECT
public:
    virtual ~WorkoutExerciseEntity();
    qint64 idExercise;
    QString name;
    qreal workWeight;
    qreal defaultWeight;
    qreal defaultWeightIncrement;
    qint64 delta;
    QStringList tags;

    QString setsAndRepsString;
    QList<WorkoutSetEntity*> setsAndReps;
    qint32 validSetCount;

public slots:
    qint32 getValidSetCount() { return validSetCount; }
    void setValidSetCount(qint32 value) { validSetCount = value; }
    bool isPrimary() { return tags.contains("primary"); }
    bool isAccessory() { return tags.contains("accessory"); }
};

#endif // WORKOUTEXERCISEENTITY_H
