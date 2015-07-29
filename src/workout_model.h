#ifndef WORKOUT_MODEL_H
#define WORKOUT_MODEL_H

#include <QObject>
#include <QAbstractItemModel>

class WorkoutTreeNode {
public:
    explicit WorkoutTreeNode(const QVariant& theData, WorkoutTreeNode* parentItem = 0) : parent(parentItem), itemData(theData) {}
    ~WorkoutTreeNode() { qDeleteAll(children); }
    void appendChild(WorkoutTreeNode* child) { children.append(child); }
    WorkoutTreeNode* child(int row) { return children.value(row); }
    int childCount() const { return children.count(); }
    int columnCount() const { return 1; }
    QVariant data() { return itemData; }
    int row() const {
        if (parent)
            return parent->children.indexOf(const_cast<WorkoutTreeNode*>(this));
        return 0;
    }
    WorkoutTreeNode* parentItem() { return parent; }
    bool setData(const QVariant& value, int role);
private:
    WorkoutTreeNode* parent;
    QList<WorkoutTreeNode*> children;
    QVariant itemData;
};

class WorkoutSetEntity : public QObject {
    Q_OBJECT
    //Q_PROPERTY(qint32 repsToDoCount MEMBER repsToDoCount NOTIFY repsToDoCountChanged)
    //Q_PROPERTY(qint32 repsDoneCount MEMBER repsDoneCount NOTIFY repsDoneCountChanged)
    //Q_PROPERTY(QString state MEMBER state NOTIFY stateChanged)
    Q_PROPERTY(qint32 repsToDoCount READ getRepsToDoCount WRITE setRepsToDoCount NOTIFY repsToDoCountChanged)
    Q_PROPERTY(qint32 repsDoneCount READ getRepsDoneCount WRITE setRepsDoneCount NOTIFY repsDoneCountChanged)
    Q_PROPERTY(QString state READ getState WRITE setState NOTIFY stateChanged)
public:
    WorkoutSetEntity() : setId(ID_NOT_SET), repsToDoCount(NO_REPS_DONE), repsDoneCount(NO_REPS_DONE), state(EMPTY_STATE) {}
    WorkoutSetEntity(qint32 newRepsToDoCount) : setId(ID_NOT_SET), repsToDoCount(newRepsToDoCount), repsDoneCount(NO_REPS_DONE), state(EMPTY_STATE) {}

    const static qint32 ID_NOT_SET = -1;
    const static qint32 NO_REPS_DONE = -1;
    const static QString EMPTY_STATE;

    qint64 setId;
    qint32 repsToDoCount;
    qint32 repsDoneCount;
    QString state;
    qint32 getRepsToDoCount() const { return repsToDoCount; }
    void setRepsToDoCount(const qint32 &value) { repsToDoCount = value; emit repsToDoCountChanged(); }
    qint32 getRepsDoneCount() const { return repsDoneCount; }
    void setRepsDoneCount(const qint32 &value) { repsDoneCount = value; emit repsDoneCountChanged(); }
    QString getState() const { return state; }
    void setState(const QString &value) { state = value; emit stateChanged(); }

public slots:
    bool isCompleted() { return state != "empty"; }
signals:
    void repsToDoCountChanged();
    void repsDoneCountChanged();
    void stateChanged();
};

class WorkoutExerciseEntity : public QObject {
    Q_OBJECT
public:
    QString name;
    qint32 defaultWeight;
    qint32 defaultWeightIncrement;
    QList<WorkoutSetEntity*> setsAndReps;
    QString setsAndRepsString;
};

class WorkoutEntity : public QObject {
    Q_OBJECT
public:
    QString name;
    QList<WorkoutExerciseEntity*> exercises;
};

class WorkoutModel : public QAbstractItemModel
{
    Q_OBJECT
    Q_PROPERTY(int exerciseCount READ exerciseCount NOTIFY exerciseCountChanged)
public:

    enum AnimalRoles {
            ExerciseNameRole = Qt::UserRole + 1,
            ExerciseSetsAndRepsRole,
            ExerciseWeightRole,
            RepsDoneCountRole,
            RepsToDoCountRole,
            RepSetStateRole
        };

    explicit WorkoutModel(QObject* parent = 0);
    ~WorkoutModel();

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

public slots:
    void get();
    int exerciseCount();
    int setsCountForExercise(int exerciseIndex);
    QVariant getSet(int exerciseIndex, int setIndex);

signals:
    void exerciseCountChanged();
protected:
    virtual QHash<int, QByteArray> roleNames() const;


private:
    WorkoutTreeNode* root;
};

#endif // WORKOUT_MODEL_H
