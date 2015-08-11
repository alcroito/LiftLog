#ifndef WORKOUT_MODEL_H
#define WORKOUT_MODEL_H

#include <QObject>
#include <QAbstractItemModel>
#include <QDate>

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
    WorkoutEntity* fetchWorkoutDataFromDB();
    WorkoutTreeNode* parseEntityToTree(WorkoutEntity* entity);

    qint64 getLastNotCompletedWorkoutId(qint64 idUser);



public slots:
    void clear();
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    int exerciseCount();
    int setsCountForExercise(int exerciseIndex);
    QVariant getSet(int exerciseIndex, int setIndex);
    QModelIndex getSetModelIndex(int exerciseIndex, int setIndex);
    void resetBlinkingSets();
    int getCompletedSetsCountForAllExercises();

    // DB manipulation methods.
    bool createStartingWorkoutEntry(qint64 idUser, qint64 day, QDateTime date, qreal userWeight);

    void getLastNotCompletedWorkoutOrCreateNew();
    bool workoutIsPartiallyCompleted(qint64 id);

    void getWorkoutOnDateOrCreateNewAtDate(QDate date);
    void getWorkoutOnDate(QDate date);

    void getWorkoutData();
    void saveWorkoutData(bool setCompleted = true);
    void deleteWorkoutData();

    void changeAndSaveStartDate(QDate date);
signals:
    void exerciseCountChanged();
    void workoutEntityChanged();

protected:
    virtual QHash<int, QByteArray> roleNames() const;

private:
    WorkoutTreeNode* root;
    WorkoutEntity* workoutEntity;
    qint64 workoutId;
    qint64 workoutDay;
};

#endif // WORKOUT_MODEL_H
