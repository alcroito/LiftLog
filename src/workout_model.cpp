#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QDateTime>

#include "workout_model.h"
#include "app_state.h"
#include "user.h"
#include "db_manager.h"

#include "workout_exercise_entity.h"
#include "workout_set_entity.h"
#include "workout_entity.h"
#include "workout_tree_node.h"

QString const WorkoutSetEntity::EMPTY_STATE = "empty";

WorkoutModel::WorkoutModel(QObject* parent) : QAbstractItemModel(parent), root(0), workoutEntity(0), idWorkout(0)
{
}

WorkoutModel::~WorkoutModel()
{
    clear();
}

void WorkoutModel::clear() {
    if (root) delete root;
    root = 0;
    if (workoutEntity) delete workoutEntity;
    workoutEntity = 0;
}

void WorkoutModel::getWorkoutData() {
    clear();

    // @TODO replace with proper logic of getting the next workout day after the last
    // previous one done, or if none was done, the first workout day of a workout template.
    int randomNumber = rand() % 2;
    workoutEntity = fetchWorkoutDataFromDB(randomNumber);
    root = parseEntityToTree(workoutEntity);
}

void WorkoutModel::saveWorkoutData()
{
    DBTransaction transaction;
    QSqlQuery query;
    bool result;

    query.prepare("UPDATE user_workout "
                  "SET date_ended = :date_ended, last_updated = :last_updated, completed = 1 "
                  "WHERE id_workout = :id_workout");

    QDateTime currentDateTime = QDateTime::currentDateTimeUtc();
    query.bindValue(":date_ended", currentDateTime);
    query.bindValue(":last_updated", currentDateTime);
    query.bindValue(":id_workout", idWorkout);

    result = query.exec();
    if (!result) {
        qDebug() << "Error updating workout data.";
        qDebug() << query.lastError();
        return;
    }

    for (int i = 0; i < workoutEntity->exercises.count(); ++i) {
        WorkoutExerciseEntity* exercise = workoutEntity->exercises.value(i);

        query.prepare("INSERT INTO user_workout_exercise_weight (id_workout, id_exercise, delta, weight)"
                      "VALUES (:id_workout, :id_exercise, :delta, :weight)");
        query.bindValue(":id_workout", idWorkout);
        query.bindValue(":id_exercise", exercise->idExercise);
        query.bindValue(":delta", exercise->delta);
        query.bindValue(":weight", exercise->workWeight);

        bool result = query.exec();
        if (!result) {
            qDebug() << "Error inserting workout exercise weight data.";
            qDebug() << query.lastError();
            return;
        }

        for (int j = 0; j < exercise->setsAndReps.count(); j++) {
            WorkoutSetEntity* set = exercise->setsAndReps.value(j);

            query.prepare("INSERT INTO user_workout_exercise_stats (id_workout, id_exercise, delta, set_number, reps_done)"
                          "VALUES (:id_workout, :id_exercise, :delta, :set_number, :reps_done)");
            query.bindValue(":id_workout", idWorkout);
            query.bindValue(":id_exercise", exercise->idExercise);
            query.bindValue(":delta", exercise->delta);
            query.bindValue(":set_number", set->getSetId());
            query.bindValue(":reps_done", set->getRepsDoneCount());

            bool result = query.exec();
            if (!result) {
                qDebug() << "Error inserting workout exercise stats data.";
                qDebug() << query.lastError();
                return;
            }

        }
    }

    transaction.commit();
}

int WorkoutModel::rowCount(const QModelIndex &parent) const
{
    WorkoutTreeNode* parentItem;
    if (parent.column() > 0) return 0;

    if (!parent.isValid())
        parentItem = root;
    else
        parentItem = static_cast<WorkoutTreeNode*>(parent.internalPointer());

    if (!parentItem) return 0;
    return parentItem->childCount();
}

int WorkoutModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<WorkoutTreeNode*>(parent.internalPointer())->columnCount();
    else
        return root->columnCount();
}

int WorkoutModel::exerciseCount()
{
    return rowCount();
}

int WorkoutModel::setsCountForExercise(int exerciseIndex)
{
    QModelIndex exerciseModelIndex = index(exerciseIndex, 0);
    return rowCount(exerciseModelIndex);
}

QVariant WorkoutModel::getSet(int exerciseIndex, int setIndex) {
    QModelIndex exerciseModelIndex = index(exerciseIndex, 0);
    QModelIndex setModelIndex = index(setIndex, 0, exerciseModelIndex);
    WorkoutTreeNode *item = static_cast<WorkoutTreeNode*>(setModelIndex.internalPointer());
    return item->data();
}

qint64 WorkoutModel::getLastNotCompletedWorkoutId(qint64 idUser) {
    qint64 id_workout = 0;

    QSqlQuery query;
    query.prepare("SELECT id_workout FROM user_workout WHERE completed = 0 AND id_user = :id_user ORDER BY id_workout DESC");
    query.bindValue(":id_user", idUser);
    bool result = query.exec();
    if (!result) {
        qDebug() << "Error getting last, not completed workout.";
        qDebug() << query.lastError();
    }
    else {
        while (query.next()) {
            if (!query.isNull(0)) {
                id_workout = query.value(0).toInt();
            }
        }
    }

    return id_workout;
}

qint64 WorkoutModel::getLastNotCompletedWorkoutOrCreateNew()
{

    qint64 id_user = AppState::getInstance()->getCurrentUser()->getId();
    qint64 id_workout = getLastNotCompletedWorkoutId(id_user);

    if (!id_workout) {
        QSqlQuery query;
        bool result;
        query.prepare("INSERT INTO user_workout (id_user, id_workout_template, date_started, date_ended, last_updated, user_weight, completed) VALUES "
                      "(:id_user, :id_workout_template, :date_started, :date_ended, :last_updated, :user_weight, :completed)");
        query.bindValue(":id_user", id_user);
        query.bindValue(":id_workout_template", 1);
        QDateTime currentDateTime = QDateTime::currentDateTimeUtc();
        query.bindValue(":date_started", currentDateTime);
        query.bindValue(":date_ended", QVariant());
        query.bindValue(":last_updated", currentDateTime);
        query.bindValue(":user_weight", 50);
        query.bindValue(":completed", false);
        result = query.exec();
        if (!result) {
            qDebug() << "Error inserting new user workout.";
            qDebug() << query.lastError();
        }
    }

    id_workout = getLastNotCompletedWorkoutId(id_user);
    idWorkout = id_workout;
    return id_workout;
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
            case RepsDoneCountRole:
                return itemData.value<WorkoutSetEntity*>()->getRepsDoneCount();
                break;
            case RepsToDoCountRole:
                return itemData.value<WorkoutSetEntity*>()->getRepsToDoCount();
                break;
            case RepSetStateRole:
                return itemData.value<WorkoutSetEntity*>()->getState();
                break;
        }
    }

    return QVariant();
}



Qt::ItemFlags WorkoutModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;
    return Qt::ItemIsEditable | QAbstractItemModel::flags(index);
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

WorkoutTreeNode* WorkoutModel::getModelItem(const QModelIndex &index) const
{
    if (index.isValid()) {
        WorkoutTreeNode* item = static_cast<WorkoutTreeNode*>(index.internalPointer());
        if (item)
            return item;
    }
    return root;
}

bool WorkoutModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    WorkoutTreeNode* item = getModelItem(index);

    QVariant itemData = item->data();
    int itemDataType = itemData.userType();

    bool result = false;

    if (itemDataType == qMetaTypeId<WorkoutExerciseEntity*>()) {
        switch (role) {
            case WorkoutModel::ExerciseNameRole:
                itemData.value<WorkoutExerciseEntity*>()->name = value.toString();
                result = true;
                break;

            case WorkoutModel::ExerciseWeightRole:
                itemData.value<WorkoutExerciseEntity*>()->defaultWeight = value.toInt();
                result = true;
                break;

            case WorkoutModel::ExerciseSetsAndRepsRole:
                itemData.value<WorkoutExerciseEntity*>()->setsAndRepsString = value.toString();
                result = true;
                break;
        }
    }
    else if (itemDataType == qMetaTypeId<WorkoutSetEntity*>()) {
        switch (role) {
            case WorkoutModel::RepsDoneCountRole:
                itemData.value<WorkoutSetEntity*>()->setRepsDoneCount(value.toInt());
                result = true;
                break;

            case WorkoutModel::RepsToDoCountRole:
                itemData.value<WorkoutSetEntity*>()->setRepsToDoCount(value.toInt());
                result = true;
                break;

            case WorkoutModel::RepSetStateRole:
                itemData.value<WorkoutSetEntity*>()->setState(value.toString());
                result = true;
                break;
        }
    }

    if (result)
        emit dataChanged(index, index);

    return result;
}

WorkoutEntity* WorkoutModel::fetchWorkoutDataFromDB(qint64 workoutDay)
{
    QSqlQuery query;
    query.prepare("SELECT wt.name AS workout_name, e.name AS exercise_name, COALESCE(uwew.weight + e.default_weight_increment, e.default_weight) AS work_weight, "
                  "e.default_weight, e.default_weight_increment, sr.set_count, sr.rep_count, e.id_exercise, wte.delta \
                  FROM workout_template wt \
                  INNER JOIN workout_template_exercises wte ON wte.id_workout_template = wt.id_workout_template \
                  INNER JOIN exercise e ON e.id_exercise = wte.id_exercise \
                  INNER JOIN set_and_rep sr ON sr.id_set_and_rep = e.id_default_set_and_rep \
                  INNER JOIN user_workout uw ON uw.id_workout_template = wt.id_workout_template \
                  LEFT JOIN user_workout_exercise_weight uwew ON uwew.id_exercise = e.id_exercise AND uwew.delta = wte.delta \
                  WHERE uw.id_workout = :id_workout AND wte.day = :workout_day \
                  ORDER BY wte.delta");
    query.bindValue(":id_workout", idWorkout);
    query.bindValue(":workout_day", workoutDay);
    bool result = query.exec();
    if (!result) {
        qDebug() << "Error getting workout data.";
        qDebug() << query.lastError();
        return 0;
    }

    QSqlQuery queryStats;
    queryStats.prepare("SELECT id_exercise, delta, set_number, reps_done "
                       "FROM user_workout_exercise_stats "
                       "WHERE id_workout = :id_workout ORDER BY id_exercise, delta, set_number");
    queryStats.bindValue(":id_workout", idWorkout);
    bool resultStats = queryStats.exec();
    if (!resultStats) {
        qDebug() << "Error getting workout stats data.";
        qDebug() << query.lastError();
        return 0;
    }

    WorkoutEntity* workout = new WorkoutEntity;
    workout->workoutId = idWorkout;

    while (query.next()) {
        WorkoutExerciseEntity* exercise = new WorkoutExerciseEntity();
        workout->name = query.value(0).toString();
        exercise->name = query.value(1).toString();
        exercise->workWeight = query.value(2).toInt();
        exercise->defaultWeight = query.value(3).toInt();
        exercise->defaultWeightIncrement = query.value(4).toInt();
        exercise->idExercise = query.value(7).toInt();
        exercise->delta = query.value(8).toInt();

        qint32 setCount = query.value(5).toInt();
        qint32 repCount = query.value(6).toInt();
        exercise->setsAndRepsString = tr("%1x%2").arg(setCount).arg(repCount);

        for (int i = 0; i < setCount; ++i) {
            queryStats.next();
            WorkoutSetEntity* set = new WorkoutSetEntity(repCount);
            if (queryStats.isValid() && !queryStats.isNull(3)) {
                int repsDone = queryStats.value(3).toInt();
                set->setRepsDoneCount(repsDone);
            }
            set->setSetId(i);
            exercise->setsAndReps.append(set);
        }
        workout->exercises.append(exercise);
    }

    return workout;
}

WorkoutTreeNode* WorkoutModel::parseEntityToTree(WorkoutEntity* entity) {
    if (!entity) return 0;
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
    roles[RepsDoneCountRole] = "repsDoneCount";
    roles[RepsToDoCountRole] = "repsToDoCount";
    roles[RepSetStateRole] = "state";

    return roles;
}
