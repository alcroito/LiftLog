#ifndef WORKOUTEXERCISEENTITY_H
#define WORKOUTEXERCISEENTITY_H

#include <QObject>
#include <QStringList>

class WorkoutSetEntity;

class WorkoutExerciseEntity : public QObject {
    Q_OBJECT
    Q_ENUMS(ExerciseCategory)
public:
    enum ExerciseCategory {
        Barbell = 0,
        BodyWeight,
        Unknown
    };

    virtual ~WorkoutExerciseEntity();
    qint64 idExercise;
    QString name;
    qreal workWeight;
    qreal defaultWeight;
    qreal defaultWeightImperial;
    qreal defaultWeightIncrement;
    qint64 delta;
    QStringList tags;


    QString setsAndRepsString;
    QList<WorkoutSetEntity*> setsAndReps;
    qint32 validSetCount;
    qint64 idSetAndRep;

    bool successful;
    bool completedAllSets;
    qint32 completedSetCount;

public slots:
    qint32 getValidSetCount() { return validSetCount; }
    void setValidSetCount(qint32 value) { validSetCount = value; }
    qint64 getIdSetAndRep() const { return idSetAndRep; }
    bool isPrimary() { return tags.contains("primary"); }
    bool isAccessory() { return tags.contains("accessory"); }
    ExerciseCategory exerciseCategory();
    qreal getBarbellWeight();
};

#endif // WORKOUTEXERCISEENTITY_H
