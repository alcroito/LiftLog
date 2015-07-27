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
private:
    WorkoutTreeNode* parent;
    QList<WorkoutTreeNode*> children;
    QVariant itemData;
};

class WorkoutSetEntity : public QObject {
    Q_OBJECT
public:
    WorkoutSetEntity() : repCount(0) {}
    WorkoutSetEntity(qint32 newRepCount) : repCount(newRepCount) {}

    qint32 repCount;
    QString state;
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
public:

    enum AnimalRoles {
            ExerciseNameRole = Qt::UserRole + 1,
            ExerciseSetsAndRepsRole,
            ExerciseWeightRole,
            RepSetCountRole,
            RepSetStateRole
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

    WorkoutEntity* fetchWorkoutDataFromDB(qint64 workoutDay = 0);
    WorkoutTreeNode* parseEntityToTree(WorkoutEntity* entity);

public slots:
    void get();
protected:
    virtual QHash<int, QByteArray> roleNames() const;
private:
    WorkoutTreeNode* root;
};

#endif // WORKOUT_MODEL_H
