#ifndef WORKOUT_MODEL_H
#define WORKOUT_MODEL_H

#include <QObject>
#include <QAbstractItemModel>

class WorkoutEntity;
class WorkoutSetEntity;
class WorkoutTreeNode;

class WorkoutModel : public QAbstractItemModel
{
    Q_OBJECT
    Q_ENUMS(WorkoutRoles)
    Q_PROPERTY(int exerciseCount READ exerciseCount NOTIFY exerciseCountChanged)
    Q_PROPERTY(WorkoutEntity* workoutEntity MEMBER workoutEntity NOTIFY workoutEntityChanged)
public:

    enum WorkoutRoles {
            WorkoutEntityRole = Qt::UserRole + 1,
            ExerciseNameRole,
            ExerciseSetsAndRepsRole,
            ExerciseWeightRole,
            ExerciseWeightIncrementRole,
            ExerciseEntityRole,
            RepsDoneCountRole,
            RepsToDoCountRole,
            RepsSetStateRole
        };

    explicit WorkoutModel(QObject* parent = 0);
    ~WorkoutModel();
    void clear();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;


    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;

    WorkoutTreeNode* getModelItem(const QModelIndex &index) const;
    WorkoutEntity* fetchWorkoutDataFromDB(qint64 workoutDay = 0);
    WorkoutTreeNode* parseEntityToTree(WorkoutEntity* entity);

    qint64 getLastNotCompletedWorkoutId(qint64 idUser);

public slots:
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    int exerciseCount();
    int setsCountForExercise(int exerciseIndex);
    QVariant getSet(int exerciseIndex, int setIndex);
    QModelIndex getSetModelIndex(int exerciseIndex, int setIndex);
    void resetBlinkingSets();

    // DB manipulation methods.
    qint64 getLastNotCompletedWorkoutOrCreateNew();
    void getWorkoutData();
    void saveWorkoutData();

signals:
    void exerciseCountChanged();
    void workoutEntityChanged();

protected:
    virtual QHash<int, QByteArray> roleNames() const;

private:
    WorkoutTreeNode* root;
    WorkoutEntity* workoutEntity;
    qint64 idWorkout;
};

#endif // WORKOUT_MODEL_H
