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
    Q_PROPERTY(int exerciseCount READ exerciseCount NOTIFY exerciseCountChanged)
public:

    enum WorkoutRoles {
            ExerciseNameRole = Qt::UserRole + 1,
            ExerciseSetsAndRepsRole,
            ExerciseWeightRole,
            RepsDoneCountRole,
            RepsToDoCountRole,
            RepSetStateRole
        };

    explicit WorkoutModel(QObject* parent = 0);
    ~WorkoutModel();
    void clear();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

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
    int exerciseCount();
    int setsCountForExercise(int exerciseIndex);
    QVariant getSet(int exerciseIndex, int setIndex);

    // DB manipulation methods.
    qint64 getLastNotCompletedWorkoutOrCreateNew();
    void getWorkoutData();
    void saveWorkoutData();

signals:
    void exerciseCountChanged();

protected:
    virtual QHash<int, QByteArray> roleNames() const;

private:
    WorkoutTreeNode* root;
    WorkoutEntity* workoutEntity;
    qint64 idWorkout;
};

#endif // WORKOUT_MODEL_H
