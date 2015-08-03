#ifndef WORKOUTEXERCISEENTITY_H
#define WORKOUTEXERCISEENTITY_H

#include <QObject>

class WorkoutSetEntity;

class WorkoutExerciseEntity : public QObject {
    Q_OBJECT
public:
    ~WorkoutExerciseEntity();
    QString name;
    qint32 workWeight;
    qint32 defaultWeight;
    qint32 defaultWeightIncrement;
    QList<WorkoutSetEntity*> setsAndReps;
    QString setsAndRepsString;
    qint64 idExercise;
    qint64 delta;
};

#endif // WORKOUTEXERCISEENTITY_H
