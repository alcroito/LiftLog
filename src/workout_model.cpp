#include "workout_model.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

WorkoutModel::WorkoutModel(QObject* parent) : QAbstractItemModel(parent), root(0)
{
}

void WorkoutModel::get() {
    // @TODO replace with proper logic of getting the next workout day after the last
    // previous one done, or if none was done, the first workout day of a workout template.
    int randomNumber = rand() % 2;
    WorkoutEntity* workoutEntity = fetchWorkoutDataFromDB(randomNumber);
    root = parseEntityToTree(workoutEntity);
}

WorkoutModel::~WorkoutModel()
{
    delete root;
}

int WorkoutModel::rowCount(const QModelIndex &parent) const
{
    WorkoutTreeNode* parentItem;
    if (parent.column() > 0) return 0;

    if (!parent.isValid())
        parentItem = root;
    else
        parentItem = static_cast<WorkoutTreeNode*>(parent.internalPointer());

    return parentItem->childCount();
}

int WorkoutModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<WorkoutTreeNode*>(parent.internalPointer())->columnCount();
    else
        return root->columnCount();
}

QVariant WorkoutModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    WorkoutTreeNode *item = static_cast<WorkoutTreeNode*>(index.internalPointer());
    if (!item) {
        return QVariant();
    }

    QVariant itemData = item->data();
    int itemDataType = itemData.userType();

    if (itemDataType == qMetaTypeId<WorkoutExerciseEntity*>()) {
        switch (role) {
            case ExerciseNameRole:
                return itemData.value<WorkoutExerciseEntity*>()->name;

            case ExerciseWeightRole:
                return itemData.value<WorkoutExerciseEntity*>()->defaultWeight;

            case ExerciseSetsAndRepsRole:
                return itemData.value<WorkoutExerciseEntity*>()->setsAndRepsString;
        }
    }

    if (itemDataType == qMetaTypeId<WorkoutSetEntity*>()) {
        switch (role) {
            case RepSetCountRole:
                return itemData.value<WorkoutSetEntity*>()->repCount;
                break;
            case RepSetStateRole:
                return itemData.value<WorkoutSetEntity*>()->state;
                break;
        }
    }

    return QVariant();
}

Qt::ItemFlags WorkoutModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;
    return QAbstractItemModel::flags(index);
}

QVariant WorkoutModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(section);
    Q_UNUSED(orientation);
    Q_UNUSED(role);
    return QVariant();
}

QModelIndex WorkoutModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    WorkoutTreeNode* parentItem;

    if (!parent.isValid())
        parentItem = root;
    else
        parentItem = static_cast<WorkoutTreeNode*>(parent.internalPointer());

    WorkoutTreeNode* childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex WorkoutModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    WorkoutTreeNode *childItem = static_cast<WorkoutTreeNode*>(index.internalPointer());
    WorkoutTreeNode *parentItem = childItem->parentItem();

    if (parentItem == root)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

WorkoutEntity* WorkoutModel::fetchWorkoutDataFromDB(qint64 workoutDay)
{
    QSqlQuery query;
    query.prepare("SELECT wt.name AS workout_name, e.name AS exercise_name, e.default_weight, e.default_weight_increment, sr.set_count, sr.rep_count, e.id_exercise \
                FROM workout_template wt \
                INNER JOIN workout_template_exercises wte ON wte.id_workout_template = wt.id_workout_template \
                INNER JOIN exercise e ON e.id_exercise = wte.id_exercise \
                INNER JOIN set_and_rep sr ON sr.id_set_and_rep = e.id_default_set_and_rep \
                WHERE wt.id_workout_template = 1 AND wte.day = :workout_day \
                ORDER BY wte.delta");
    query.bindValue(":workout_day", workoutDay);
    bool result = query.exec();
    if (!result) {
        qDebug() << "Error getting workout data";
        qDebug() << query.lastError();
        return 0;
    }

    // @TODO Fix memory management.
    WorkoutEntity* workout = new WorkoutEntity;

    while (query.next()) {
        WorkoutExerciseEntity* exercise = new WorkoutExerciseEntity();
        workout->name = query.value(0).toString();
        exercise->name = query.value(1).toString();
        exercise->defaultWeight = query.value(2).toInt();
        exercise->defaultWeightIncrement = query.value(3).toInt();

        qint32 setCount = query.value(4).toInt();
        qint32 repCount = query.value(5).toInt();
        exercise->setsAndRepsString = tr("%1x%2").arg(setCount).arg(repCount);

        for (int i = 0; i < setCount; ++i) {
            WorkoutSetEntity* set = new WorkoutSetEntity(repCount);
            set->state = "empty";
            exercise->setsAndReps.append(set);
        }
        workout->exercises.append(exercise);
    }

    return workout;
}

WorkoutTreeNode* WorkoutModel::parseEntityToTree(WorkoutEntity* entity) {
    WorkoutTreeNode* top = new WorkoutTreeNode(QVariant::fromValue(entity));
    for (int i = 0; i < entity->exercises.count(); ++i) {
        WorkoutExerciseEntity* exercise = entity->exercises.value(i);
        WorkoutTreeNode* exerciseNode = new WorkoutTreeNode(QVariant::fromValue(exercise), top);
        for (int j = 0; j < exercise->setsAndReps.count(); j++) {
            WorkoutSetEntity* set = exercise->setsAndReps.value(j);
            WorkoutTreeNode* setNode = new WorkoutTreeNode(QVariant::fromValue(set), exerciseNode);
            exerciseNode->appendChild(setNode);
        }
        top->appendChild(exerciseNode);
    }
    return top;
}

QHash<int, QByteArray> WorkoutModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[ExerciseNameRole] = "name";
    roles[ExerciseSetsAndRepsRole] = "setsAndReps";
    roles[ExerciseWeightRole] = "weight";
    roles[RepSetCountRole] = "repCount";
    roles[RepSetStateRole] = "state";

    return roles;
}
