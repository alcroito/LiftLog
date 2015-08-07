#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QDateTime>
#include <QQmlEngine>

#include "workout_model.h"
#include "app_state.h"
#include "user.h"
#include "db_manager.h"

#include "workout_exercise_entity.h"
#include "workout_set_entity.h"
#include "workout_entity.h"
#include "workout_tree_node.h"

WorkoutModel::WorkoutModel(QObject* parent) : QAbstractItemModel(parent), root(0), workoutEntity(0), workoutId(0), workoutDay(0)
{
}

WorkoutModel::~WorkoutModel()
{
    clear();
}

void WorkoutModel::clear() {
    if (root) delete root;
    root = 0;
    if (workoutEntity) {
        delete workoutEntity;
    }
    workoutEntity = 0;
    workoutId = 0;
    workoutDay = 0;
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

    if (itemDataType == qMetaTypeId<WorkoutEntity*>()) {
        switch (role) {
            case WorkoutEntityRole:
                WorkoutEntity* entity = qvariant_cast<WorkoutEntity*>(itemData);
                QQmlEngine::setObjectOwnership(entity, QQmlEngine::CppOwnership);
                return itemData;
        }
    }

    if (itemDataType == qMetaTypeId<WorkoutExerciseEntity*>()) {
        switch (role) {
            case ExerciseNameRole:
                return itemData.value<WorkoutExerciseEntity*>()->name;

            case ExerciseWeightRole:
                return itemData.value<WorkoutExerciseEntity*>()->workWeight;

            case ExerciseWeightIncrementRole:
                return itemData.value<WorkoutExerciseEntity*>()->defaultWeightIncrement;

            case ExerciseSetsAndRepsRole:
                return itemData.value<WorkoutExerciseEntity*>()->setsAndRepsString;

            case ExerciseEntityRole:
                WorkoutExerciseEntity* entity = qvariant_cast<WorkoutExerciseEntity*>(itemData);
                QQmlEngine::setObjectOwnership(entity, QQmlEngine::CppOwnership);
                return itemData;
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
            case RepsSetStateRole:
                return itemData.value<WorkoutSetEntity*>()->getState();
                break;
        }
    }

    return QVariant();
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

            case WorkoutModel::RepsSetStateRole:
                itemData.value<WorkoutSetEntity*>()->setState(value.toString());
                result = true;
                break;
        }
    }

    if (result)
        emit dataChanged(index, index);

    return result;
}

QHash<int, QByteArray> WorkoutModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[WorkoutEntityRole] = "workoutEntity";
    roles[ExerciseNameRole] = "name";
    roles[ExerciseSetsAndRepsRole] = "setsAndReps";
    roles[ExerciseWeightRole] = "weight";
    roles[ExerciseWeightIncrementRole] = "weightIncrement";
    roles[ExerciseEntityRole] = "exerciseEntity";
    roles[RepsDoneCountRole] = "repsDoneCount";
    roles[RepsToDoCountRole] = "repsToDoCount";
    roles[RepsSetStateRole] = "state";

    return roles;
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

QModelIndex WorkoutModel::getSetModelIndex(int exerciseIndex, int setIndex) {
    QModelIndex exerciseModelIndex = index(exerciseIndex, 0);
    QModelIndex setModelIndex = index(setIndex, 0, exerciseModelIndex);
    return setModelIndex;
}

QVariant WorkoutModel::getSet(int exerciseIndex, int setIndex) {
    QModelIndex setModelIndex = getSetModelIndex(exerciseIndex, setIndex);
    WorkoutTreeNode *item = static_cast<WorkoutTreeNode*>(setModelIndex.internalPointer());
    WorkoutSetEntity* set = qvariant_cast<WorkoutSetEntity*>(item->data());
    QQmlEngine::setObjectOwnership(set, QQmlEngine::CppOwnership);
    return QVariant::fromValue(set);
}

void WorkoutModel::resetBlinkingSets()
{
    for (int i = 0; i < workoutEntity->exercises.count(); ++i) {
        WorkoutExerciseEntity* exercise = workoutEntity->exercises.value(i);
        for (int j = 0; j < exercise->setsAndReps.count(); j++) {
            WorkoutSetEntity* set = exercise->setsAndReps.value(j);
            QModelIndex index = getSetModelIndex(i, j);
            if (set->isBlinking()) {
                setData(index, WorkoutSetEntity::EMPTY_STATE, RepsSetStateRole);
            }
        }
    }
}

int WorkoutModel::getCompletedSetsCountForAllExercises()
{
    int completed = 0;

    for (int i = 0; i < workoutEntity->exercises.count(); ++i) {
        WorkoutExerciseEntity* exercise = workoutEntity->exercises.value(i);
        for (int j = 0; j < exercise->setsAndReps.count(); j++) {
            WorkoutSetEntity* set = exercise->setsAndReps.value(j);
            if (set->isAttempted()) {
                completed++;
            }
        }
    }

    return completed;
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

bool WorkoutModel::createStartingWorkoutEntry(qint64 idUser, qint64 day, QDateTime date, qreal userWeight) {
    QSqlQuery query;
    query.prepare("INSERT INTO user_workout (id_user, id_workout_template, day, date_started, date_ended, last_updated, user_weight, completed) VALUES "
                  "(:id_user, :id_workout_template, :day, :date_started, :date_ended, :last_updated, :user_weight, :completed)");

    query.bindValue(":id_user", idUser);

    int workoutTemplateId = AppState::getInstance()->getCurrentUser()->getLastIdWorkoutTemplate();
    query.bindValue(":id_workout_template", workoutTemplateId);

    query.bindValue(":day", day);
    query.bindValue(":date_started", date);
    query.bindValue(":date_ended", QVariant());
    query.bindValue(":last_updated", date);
    query.bindValue(":user_weight", userWeight);
    query.bindValue(":completed", false);

    bool result = query.exec();
    if (!result) {
        qDebug() << "Error inserting new user workout.";
        qDebug() << query.lastError();
    }

    return result;
}

void WorkoutModel::getLastNotCompletedWorkoutOrCreateNew()
{
    qint64 id_user = AppState::getInstance()->getCurrentUser()->getId();
    qint64 id_workout = getLastNotCompletedWorkoutId(id_user);
    qint64 day = AppState::getInstance()->getCurrentUser()->getNextWorkoutTemplateDay();
    qreal weight = AppState::getInstance()->getCurrentUser()->getUserWeight();

    if (!id_workout) {
        QDateTime current = QDateTime::currentDateTimeUtc();
        createStartingWorkoutEntry(id_user, day, current, weight);
    }

    id_workout = getLastNotCompletedWorkoutId(id_user);
    workoutId = id_workout;
    workoutDay = AppState::getInstance()->getCurrentUser()->getNextWorkoutTemplateDay();
}

void WorkoutModel::getWorkoutOnDate(QDate date)
{
     qint64 idUser = AppState::getInstance()->getCurrentUser()->getId();

     QSqlQuery query;
     QString queryString = "SELECT id_workout, day "
                           "FROM user_workout "
                           "WHERE strftime('%Y-%m-%d', date_started) = strftime('%Y-%m-%d', '%1') "
                           "AND id_user = :id_user "
                           "ORDER BY date_started DESC LIMIT 1";
     queryString = queryString.arg(date.toString("yyyy-MM-dd"));
     query.prepare(queryString);

     query.bindValue(":id_user", idUser);

     bool result = query.exec();

     if (!result) {
         qDebug() << "Error getting last workout for given date.";
         qDebug() << query.lastError();
     }
     else {
         while (query.next()) {
             if (!query.isNull(0)) {
                 workoutId = query.value(0).toInt();
                 workoutDay = query.value(1).toInt();
             }
         }
     }
}

void WorkoutModel::getWorkoutOnDateOrCreateNewAtDate(QDate date) {
    getWorkoutOnDate(date);
    if (!workoutId) {
        QTime time(12, 0, 0, 0);
        QDateTime dateTime(date, time, Qt::UTC);
        qint64 idUser = AppState::getInstance()->getCurrentUser()->getId();
        qint64 day = AppState::getInstance()->getCurrentUser()->getNextWorkoutTemplateDay();
        qreal weight = AppState::getInstance()->getCurrentUser()->getUserWeight();
        createStartingWorkoutEntry(idUser, day, dateTime, weight);
        getWorkoutOnDate(date);
    }
}

WorkoutEntity* WorkoutModel::fetchWorkoutDataFromDB()
{
    QSqlQuery query;
    query.prepare("SELECT wt.name AS workout_name, e.name AS exercise_name, COALESCE(uwew.weight + e.default_weight_increment, e.default_weight) AS work_weight, "
                  "e.default_weight, e.default_weight_increment, sr.set_count, sr.rep_count, e.id_exercise, wte.delta, uw.id_workout_template, \
                  uw.day, uw.date_started, uw.date_ended, uw.last_updated, uw.user_weight, uw.completed, e.tags \
                  FROM workout_template wt \
                  INNER JOIN workout_template_exercises wte ON wte.id_workout_template = wt.id_workout_template \
                  INNER JOIN exercise e ON e.id_exercise = wte.id_exercise \
                  INNER JOIN set_and_rep sr ON sr.id_set_and_rep = e.id_default_set_and_rep \
                  INNER JOIN user_workout uw ON uw.id_workout_template = wt.id_workout_template \
                  LEFT JOIN user_workout_exercise_weight uwew ON uwew.id_exercise = e.id_exercise AND uwew.delta = wte.delta AND uwew.id_workout = \
                    /* Extracts the id_workout of previous workout with the same day, to use as a source for completed weights */ \
                    (SELECT uw2.id_workout \
                    FROM user_workout uw1 \
                    INNER JOIN user_workout uw2 ON uw2.date_started < uw1.date_started AND uw2.day = uw1.day  \
                    WHERE uw1.id_workout = uw.id_workout \
                    ORDER BY uw2.id_workout DESC \
                    LIMIT 1) \
                  WHERE uw.id_workout = :id_workout AND wte.day = :workout_day \
                  ORDER BY wte.delta");
    query.bindValue(":id_workout", workoutId);
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
                       "WHERE id_workout = :id_workout ORDER BY delta, set_number");
    queryStats.bindValue(":id_workout", workoutId);
    bool resultStats = queryStats.exec();
    if (!resultStats) {
        qDebug() << "Error getting workout stats data.";
        qDebug() << query.lastError();
        return 0;
    }

    WorkoutEntity* workout = new WorkoutEntity;
    workout->workoutId = workoutId;
    workout->shouldBeSaved = false;
    workout->forceSave = false;
    bool first = true;
    bool firstSet = true;

    while (query.next()) {
        if (first) {
            workout->workoutTemplateId = query.value(9).toInt();
            workout->day = query.value(10).toInt();
            workout->dateStarted = query.value(11).toDateTime();
            workout->dateEnded = query.value(12).toDateTime();

            // Force save if workout was explicitly saved by pressing save button.
            if (workout->dateEnded.isValid()) {
                workout->forceSave = true;
            }
            workout->lastUpdated = query.value(13).toDateTime();
            workout->userWeight = query.value(14).toReal();
            workout->completed = query.value(15).toBool();
            first = false;
        }

        WorkoutExerciseEntity* exercise = new WorkoutExerciseEntity();
        workout->name = query.value(0).toString();
        exercise->name = query.value(1).toString();
        exercise->workWeight = query.value(2).toReal();
        exercise->defaultWeight = query.value(3).toReal();
        exercise->defaultWeightIncrement = query.value(4).toReal();
        exercise->idExercise = query.value(7).toInt();
        exercise->delta = query.value(8).toInt();
        exercise->tags = query.value(16).toString().split(",");

        // Display at least 5 set circles. Show crossed sets
        // in case if the sets should not be completed.
        qint32 setCount = query.value(5).toInt();
        qint32 minSetCount = 5;
        qint32 setsToCreate = std::max(minSetCount, setCount);

        qint32 repCount = query.value(6).toInt();
        exercise->setsAndRepsString = tr("%1x%2").arg(setCount).arg(repCount);

        for (int i = 0; i < setsToCreate; ++i) {
            WorkoutSetEntity* set = new WorkoutSetEntity(repCount);
            set->setSetId(i);

            // First set should blink to attract attention.
            if (firstSet) {
                set->setState(WorkoutSetEntity::BLINKING_STATE);
                firstSet = false;
            }

            // Display sets as crossed, which should not be completed.
            if (i >= setCount) {
                set->setState(WorkoutSetEntity::CROSSED_STATE);
                set->setRepsToDoCount(WorkoutSetEntity::NO_REPS_DONE);
            }
            else {
                // Only go to next stats row if current set is valid.
                queryStats.next();
            }

            // Pre-populate already done sets.
            if (!set->isInvalid() && queryStats.isValid() && !queryStats.isNull(3)) {
                int repsDone = queryStats.value(3).toInt();
                set->setRepsDoneCount(repsDone);
                if (repsDone != WorkoutSetEntity::NO_REPS_DONE) {
                    set->setState(WorkoutSetEntity::ACTIVE_STATE);
                }
            }

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

void WorkoutModel::getWorkoutData() {
    workoutEntity = fetchWorkoutDataFromDB();
    root = parseEntityToTree(workoutEntity);
}

void WorkoutModel::saveWorkoutData()
{
    DBTransaction transaction;
    QSqlQuery query;
    bool result;

    // Update the workout to being completed, and set the appropriate dates.
    if (!workoutEntity->completed) {
        query.prepare("UPDATE user_workout "
                      "SET date_ended = :date_ended, last_updated = :last_updated, user_weight = :user_weight, completed = 1 "
                      "WHERE id_workout = :id_workout");

        QDateTime currentDateTime = QDateTime::currentDateTimeUtc();
        query.bindValue(":date_ended", currentDateTime);
        query.bindValue(":last_updated", currentDateTime);
        query.bindValue(":user_weight", workoutEntity->userWeight);
        query.bindValue(":id_workout", workoutId);

        result = query.exec();
        if (!result) {
            qDebug() << "Error updating workout data.";
            qDebug() << query.lastError();
            return;
        }
    }
    // Just update the last updated date.
    else {
        query.prepare("UPDATE user_workout "
                      "SET last_updated = :last_updated, user_weight = :user_weight "
                      "WHERE id_workout = :id_workout");

        QDateTime currentDateTime = QDateTime::currentDateTimeUtc();
        query.bindValue(":last_updated", currentDateTime);
        query.bindValue(":user_weight", workoutEntity->userWeight);
        query.bindValue(":id_workout", workoutId);

        result = query.exec();
        if (!result) {
            qDebug() << "Error updating workout data.";
            qDebug() << query.lastError();
            return;
        }
    }


    // Only do the following when creating a new workout, not when updating an existing one.
    if (!workoutEntity->completed) {
        // Gets max day for workout template, to re-start the day numbering
        // in case the last workout was the last day.
        query.prepare("SELECT MAX(wte.day) "
                      "FROM workout_template_exercises wte "
                      "WHERE wte.id_workout_template = :id_workout_template");

        query.bindValue(":id_workout_template", AppState::getInstance()->getCurrentUser()->getLastIdWorkoutTemplate());

        result = query.exec();
        if (!result) {
            qDebug() << "Error getting max template day for workout.";
            qDebug() << query.lastError();
            return;
        }

        // Save the user next workout day, the last workout id used, and any other changes that were done to the user.
        User* user = AppState::getInstance()->getCurrentUser();

        qint64 newWorkoutDay = AppState::getInstance()->getCurrentUser()->getNextWorkoutTemplateDay();
        if (query.next() && !query.isNull(0)) {
            qint64 maxWorkoutDay = query.value(0).toInt() + 1;
            newWorkoutDay = (newWorkoutDay + 1) % maxWorkoutDay;
            user->setNextWorkoutTemplateDay(newWorkoutDay);
        }

        user->save();
    } else {
        // Save the last workout id used, and any other changes that were done to the user.
        User* user = AppState::getInstance()->getCurrentUser();
        user->save();
    }

    // Save exercise stats.
    for (int i = 0; i < workoutEntity->exercises.count(); ++i) {
        WorkoutExerciseEntity* exercise = workoutEntity->exercises.value(i);

        if (!workoutEntity->completed) {
            query.prepare("INSERT INTO user_workout_exercise_weight (id_workout, id_exercise, delta, weight)"
                          "VALUES (:id_workout, :id_exercise, :delta, :weight)");
            query.bindValue(":id_workout", workoutId);
            query.bindValue(":id_exercise", exercise->idExercise);
            query.bindValue(":delta", exercise->delta);
            query.bindValue(":weight", exercise->workWeight);

            bool result = query.exec();
            if (!result) {
                qDebug() << "Error saving workout exercise weight data.";
                qDebug() << query.lastError();
                return;
            }
        } else {
            query.prepare("UPDATE user_workout_exercise_weight "
                          "SET weight = :weight "
                          "WHERE id_workout = :id_workout AND id_exercise = :id_exercise AND delta = :delta");
            query.bindValue(":id_workout", workoutId);
            query.bindValue(":id_exercise", exercise->idExercise);
            query.bindValue(":delta", exercise->delta);
            query.bindValue(":weight", exercise->workWeight);

            bool result = query.exec();
            if (!result) {
                qDebug() << "Error saving workout exercise weight data.";
                qDebug() << query.lastError();
                return;
            }
        }

        for (int j = 0; j < exercise->setsAndReps.count(); j++) {
            WorkoutSetEntity* set = exercise->setsAndReps.value(j);

            // If next set is an invalid one (crossed for example, don't save it, nor the next ones).
            if (set->isInvalid()) break;

            if (!workoutEntity->completed) {
                query.prepare("INSERT INTO user_workout_exercise_stats (id_workout, id_exercise, delta, set_number, reps_done)"
                              "VALUES (:id_workout, :id_exercise, :delta, :set_number, :reps_done)");
                query.bindValue(":id_workout", workoutId);
                query.bindValue(":id_exercise", exercise->idExercise);
                query.bindValue(":delta", exercise->delta);
                query.bindValue(":set_number", set->getSetId());
                query.bindValue(":reps_done", set->getRepsDoneCount());
            }
            else {
                query.prepare("UPDATE user_workout_exercise_stats "
                              "SET reps_done = :reps_done "
                              "WHERE id_workout = :id_workout AND id_exercise = :id_exercise AND delta = :delta AND set_number = :set_number");
                query.bindValue(":id_workout", workoutId);
                query.bindValue(":id_exercise", exercise->idExercise);
                query.bindValue(":delta", exercise->delta);
                query.bindValue(":set_number", set->getSetId());
                query.bindValue(":reps_done", set->getRepsDoneCount());
            }

            bool result = query.exec();
            if (!result) {
                qDebug() << "Error saving workout exercise stats data.";
                qDebug() << query.lastError();
                return;
            }
        }
    }

    transaction.commit();
}

void WorkoutModel::deleteWorkoutData()
{
    if (!workoutId) {
        qDebug() << "Can't delete a workout that has no id.";
        return;
    }

    QSqlQuery query;
    bool result;

    query.prepare("DELETE FROM user_workout "
                  "WHERE id_workout = :id_workout");

    query.bindValue(":id_workout", workoutId);

    result = query.exec();
    if (!result) {
        qDebug() << "Error deleting workout data.";
        qDebug() << query.lastError();
        return;
    }

    query.prepare("DELETE FROM user_workout_exercise_weight "
                  "WHERE id_workout = :id_workout");

    query.bindValue(":id_workout", workoutId);

    result = query.exec();
    if (!result) {
        qDebug() << "Error deleting workout weight stats.";
        qDebug() << query.lastError();
        return;
    }

    query.prepare("DELETE FROM user_workout_exercise_stats "
                  "WHERE id_workout = :id_workout");

    query.bindValue(":id_workout", workoutId);

    result = query.exec();
    if (!result) {
        qDebug() << "Error deleting workout stats.";
        qDebug() << query.lastError();
        return;
    }
}
