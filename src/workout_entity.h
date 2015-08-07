#ifndef WORKOUTENTITY_H
#define WORKOUTENTITY_H

#include <QObject>
#include <QList>
#include <QDateTime>

class WorkoutExerciseEntity;

class WorkoutEntity : public QObject {
    Q_OBJECT
    Q_PROPERTY(qreal userWeight READ getUserWeight WRITE setUserWeight NOTIFY userWeightChanged)
    Q_PROPERTY(bool completed MEMBER completed NOTIFY completedChanged)
    Q_PROPERTY(bool shouldBeSaved MEMBER shouldBeSaved NOTIFY shouldBeSavedChanged)
    Q_PROPERTY(bool forceSave MEMBER forceSave NOTIFY forceSaveChanged)
    Q_PROPERTY(QDateTime dateStarted MEMBER dateStarted NOTIFY dateStartedChanged)
public:
    virtual ~WorkoutEntity();
    qint64 workoutId;
    qint64 workoutTemplateId;
    qint64 day;
    QString name;
    QDateTime dateStarted;
    QDateTime dateEnded;
    QDateTime lastUpdated;
    qreal userWeight;
    bool completed;
    bool shouldBeSaved;
    bool forceSave;
    QList<WorkoutExerciseEntity*> exercises;

    static const qreal initialUserWeight;

    qreal getUserWeight() { return userWeight; }
    void setUserWeight(qreal value) {
        if (value != userWeight) {
            userWeight = value;
            emit userWeightChanged(value);
        }
    }

signals:
    void userWeightChanged(qreal);
    void completedChanged();
    void shouldBeSavedChanged();
    void forceSaveChanged();
    void dateStartedChanged();
};

#endif // WORKOUTENTITY_H
