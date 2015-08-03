#ifndef WORKOUTENTITY_H
#define WORKOUTENTITY_H

#include <QObject>
#include <QList>

class WorkoutExerciseEntity;

class WorkoutEntity : public QObject {
    Q_OBJECT
public:
    ~WorkoutEntity();
    qint64 workoutTemplateId;
    qint64 workoutId;
    QString name;
    QList<WorkoutExerciseEntity*> exercises;
};

#endif // WORKOUTENTITY_H
