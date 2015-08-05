#ifndef WORKOUTSETENTITY_H
#define WORKOUTSETENTITY_H

#include <QObject>

class WorkoutSetEntity : public QObject {
    Q_OBJECT

    Q_PROPERTY(qint32 setId READ getSetId WRITE setSetId NOTIFY setIdChanged)
    Q_PROPERTY(qint32 repsToDoCount READ getRepsToDoCount WRITE setRepsToDoCount NOTIFY repsToDoCountChanged)
    Q_PROPERTY(qint32 repsDoneCount READ getRepsDoneCount WRITE setRepsDoneCount NOTIFY repsDoneCountChanged)
    Q_PROPERTY(QString state READ getState WRITE setState NOTIFY stateChanged)
public:
    WorkoutSetEntity() : setId(ID_NOT_SET), repsToDoCount(NO_REPS_DONE), repsDoneCount(NO_REPS_DONE), state(EMPTY_STATE) {}
    WorkoutSetEntity(qint32 newRepsToDoCount) : setId(ID_NOT_SET), repsToDoCount(newRepsToDoCount), repsDoneCount(NO_REPS_DONE), state(EMPTY_STATE) {}

    const static qint64 ID_NOT_SET;
    const static qint32 NO_REPS_DONE;
    const static QString EMPTY_STATE;
    const static QString BLINKING_STATE;
    const static QString CROSSED_STATE;

    qint64 getSetId() const { return setId; }
    void setSetId(const qint64 &value) { setId = value; emit setIdChanged(); }
    qint32 getRepsToDoCount() const { return repsToDoCount; }
    void setRepsToDoCount(const qint32 &value) { repsToDoCount = value; emit repsToDoCountChanged(); }
    qint32 getRepsDoneCount() const { return repsDoneCount; }
    void setRepsDoneCount(const qint32 &value) { repsDoneCount = value; emit repsDoneCountChanged(); }
    QString getState() const { return state; }
    void setState(const QString &value) { state = value; emit stateChanged(state); }

    bool isInvalid() { return setId == ID_NOT_SET || state == CROSSED_STATE; }
    bool isBlinking() { return state == BLINKING_STATE; }

public slots:
    bool isCompleted() { return state != "empty"; }
    bool isSuccessful() { return repsDoneCount == repsToDoCount; }
signals:
    void repsToDoCountChanged();
    void repsDoneCountChanged();
    void stateChanged(QString value);
    void setIdChanged();

private:
    qint64 setId;
    qint32 repsToDoCount;
    qint32 repsDoneCount;
    QString state;
};

#endif // WORKOUTSETENTITY_H
