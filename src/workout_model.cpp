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
#include "generic_tree_node.h"
#include "set_and_rep_model.h"

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
    GenericTreeNode* parentItem;
    if (parent.column() > 0) return 0;

    if (!parent.isValid())
        parentItem = root;
    else
        parentItem = static_cast<GenericTreeNode*>(parent.internalPointer());

    if (!parentItem) return 0;
    return parentItem->childCount();
}

int WorkoutModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<GenericTreeNode*>(parent.internalPointer())->columnCount();
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

    GenericTreeNode* parentItem;

    if (!parent.isValid())
        parentItem = root;
    else
        parentItem = static_cast<GenericTreeNode*>(parent.internalPointer());

    GenericTreeNode* childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex WorkoutModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    GenericTreeNode *childItem = static_cast<GenericTreeNode*>(index.internalPointer());
    GenericTreeNode *parentItem = childItem->parentItem();

    if (parentItem == root)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

GenericTreeNode* WorkoutModel::getModelItem(const QModelIndex &index) const
{
    if (index.isValid()) {
        GenericTreeNode* item = static_cast<GenericTreeNode*>(index.internalPointer());
        if (item)
            return item;
    }
    return root;
}

QVariant WorkoutModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    GenericTreeNode *item = static_cast<GenericTreeNode*>(index.internalPointer());
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

            case ExerciseWeightIncrementRole: {
                User::WeightSystem system = AppState::getInstance()->getCurrentUser()->getWeightSystem();
                // For imperial system, the weight increment should be double, because we will compute
                // using imperial units, rather than metric, and then convert to imperial.
                if (system == User::Imperial)
                    return itemData.value<WorkoutExerciseEntity*>()->defaultWeightIncrement * 2;
                return itemData.value<WorkoutExerciseEntity*>()->defaultWeightIncrement;
            }


            case ExerciseSetsAndRepsRole:
                return itemData.value<WorkoutExerciseEntity*>()->setsAndRepsString;

            case ExerciseSetAndRepIdRole:
                return itemData.value<WorkoutExerciseEntity*>()->idSetAndRep;

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
    GenericTreeNode* item = getModelItem(index);

    QVariant itemData = item->data();
    int itemDataType = itemData.userType();

    bool result = false;

    if (itemDataType == qMetaTypeId<WorkoutExerciseEntity*>()) {
        switch (role) {
            case ExerciseNameRole:
                itemData.value<WorkoutExerciseEntity*>()->name = value.toString();
                result = true;
                break;

            case ExerciseWeightRole:
                itemData.value<WorkoutExerciseEntity*>()->workWeight = value.toReal();
                result = true;
                break;

            case ExerciseSetsAndRepsRole:
                itemData.value<WorkoutExerciseEntity*>()->setsAndRepsString = value.toString();
                result = true;
                break;

            case ExerciseSetAndRepIdRole:
                itemData.value<WorkoutExerciseEntity*>()->idSetAndRep = value.toInt();
                result = true;
                break;

            case ExerciseSuccessfulRole:
                itemData.value<WorkoutExerciseEntity*>()->successful = value.toBool();
                result = true;
                break;

            case ExerciseCompletedAllSetsRole:
                itemData.value<WorkoutExerciseEntity*>()->completedAllSets = value.toBool();
                result = true;
                break;

            case ExerciseCompletedSetCountRole:
                itemData.value<WorkoutExerciseEntity*>()->completedSetCount = value.toInt();
                result = true;
                break;
        }
    }
    else if (itemDataType == qMetaTypeId<WorkoutSetEntity*>()) {
        switch (role) {
            case RepsDoneCountRole:
                itemData.value<WorkoutSetEntity*>()->setRepsDoneCount(value.toInt());
                result = true;
                break;

            case RepsToDoCountRole:
                itemData.value<WorkoutSetEntity*>()->setRepsToDoCount(value.toInt());
                result = true;
                break;

            case RepsSetStateRole:
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
    roles[ExerciseSetAndRepIdRole] = "exerciseSetAndRepId";
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
    return workoutEntity->exercises[exerciseIndex]->getValidSetCount();
}

QModelIndex WorkoutModel::getExerciseModelIndex(int exerciseIndex) {
    QModelIndex exerciseModelIndex = index(exerciseIndex, 0);
    return exerciseModelIndex;
}

QModelIndex WorkoutModel::getSetModelIndex(int exerciseIndex, int setIndex) {
    QModelIndex exerciseModelIndex = index(exerciseIndex, 0);
    QModelIndex setModelIndex = index(setIndex, 0, exerciseModelIndex);
    return setModelIndex;
}

QVariant WorkoutModel::getSet(int exerciseIndex, int setIndex) {
    QModelIndex setModelIndex = getSetModelIndex(exerciseIndex, setIndex);
    GenericTreeNode *item = static_cast<GenericTreeNode*>(setModelIndex.internalPointer());
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
            if (set->isBlinking()) {
                QModelIndex index = getSetModelIndex(i, j);
                setData(index, WorkoutSetEntity::EMPTY_STATE, RepsSetStateRole);
            }
        }
    }
}

ExerciseAndSetIndexPair* WorkoutModel::getBlinkingSetId() {
    ExerciseAndSetIndexPair* pair = new ExerciseAndSetIndexPair();

    for (int i = 0; i < workoutEntity->exercises.count(); ++i) {
        WorkoutExerciseEntity* exercise = workoutEntity->exercises.value(i);
        for (int j = 0; j < exercise->setsAndReps.count(); j++) {
            WorkoutSetEntity* set = exercise->setsAndReps.value(j);
            if (set->isBlinking()) {
                pair->exerciseIndex = i;
                pair->setIndex = j;
            }
        }
    }

    // Object will be garbage collected (deleted) by QML engine, because it will have QML ownership,
    // because we return the pointer via a slot invoked by QML.
    return pair;
}

int WorkoutModel::getCompletedSetsCountForAllExercises()
{
    int completed = 0;
    if (!workoutEntity) return completed;

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

/**
 * @brief WorkoutModel::workoutIsPartiallyCompleted
 * A workout is partially completed if it has any exercise stats saved to the DB.
 * @param id
 * @return
 */
bool WorkoutModel::workoutIsPartiallyCompleted(qint64 id)
{
    bool isPartiallyCompleted = false;
    QSqlQuery query;
    query.prepare("SELECT uw.id_workout "
                  "FROM user_workout_exercise_set_stats uwes "
                  "INNER JOIN user_workout uw ON uwes.id_workout = uw.id_workout "
                  "WHERE uw.id_workout = :id_workout "
                  "LIMIT 1");

    query.bindValue(":id_workout", id);
    bool result = query.exec();
    if (!result) {
        qDebug() << "Error checking if workout is partially completed.";
        qDebug() << query.lastError();
    }
    else {
        while (query.next()) {
            if (!query.isNull(0)) {
                isPartiallyCompleted = true;
            }
        }
    }

    return isPartiallyCompleted;
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

void WorkoutModel::reInitializeExerciseSets() {
    bool firstSet = true;
    beginResetModel();
    resetBlinkingSets();
    for (int i = 0; i < workoutEntity->exercises.count(); ++i) {
        // Get exercise.
        WorkoutExerciseEntity* exercise = workoutEntity->exercises[i];

        // Get set and rep used by this exercise.
        SetAndRep setAndRep = SetAndRepModel::getSetAndRepById(exercise->idSetAndRep);

        // Get exercise tree node, using which we will add / remove children set nodes.
        QModelIndex exerciseModelIndex = getExerciseModelIndex(i);
        GenericTreeNode* exerciseWorkoutTreeNode = static_cast<GenericTreeNode*>(exerciseModelIndex.internalPointer());

        // Set the set count.
        qint32 setCount = setAndRep.getSetCount();
        exercise->validSetCount = setCount;

        // Set the sets and reps string for the exercise.
        qint32 repCount = setAndRep.getRepCount();
        setData(exerciseModelIndex, tr("%1x%2").arg(setCount).arg(repCount), ExerciseSetsAndRepsRole);

        // Compute how many sets should exist.
        qint32 minSetCount = 5;
        qint32 setsCountThatShouldExist = std::max(minSetCount, setCount);

        // Compute how many sets should be removed / added.
        qint32 existingCreatedSetsCount = exercise->setsAndReps.count();
        qint32 setsToRemove = std::max(existingCreatedSetsCount - setsCountThatShouldExist, 0);
        qint32 setsToAdd = std::max(setsCountThatShouldExist - existingCreatedSetsCount, 0);

        for (int j = 0; j < setsToRemove; ++j) {
            delete exercise->setsAndReps.last();
            exercise->setsAndReps.removeLast();
            exerciseWorkoutTreeNode->deleteChild(exercise->setsAndReps.count());
        }

        for (int j = 0; j < setsToAdd; ++j) {
            WorkoutSetEntity* set = new WorkoutSetEntity(repCount);
            exercise->setsAndReps.append(set);
            GenericTreeNode* setNode = new GenericTreeNode(QVariant::fromValue(set), exerciseWorkoutTreeNode);
            exerciseWorkoutTreeNode->appendChild(setNode);
        }

        for (int j = 0; j < setsCountThatShouldExist; ++j) {
            WorkoutSetEntity* set = exercise->setsAndReps[j];
            set->setSetId(j);
            set->setRepsToDoCount(repCount);

            // Display sets as crossed, which should not be completed.
            if (j >= setCount) {
                set->setState(WorkoutSetEntity::CROSSED_STATE);
                set->setRepsToDoCount(WorkoutSetEntity::NO_REPS_DONE);
            } else {
                // Reset previously crossed states to emtpy state.
                if (set->getState() == WorkoutSetEntity::CROSSED_STATE) {
                    set->setState(WorkoutSetEntity::EMPTY_STATE);
                    set->setRepsDoneCount(WorkoutSetEntity::NO_REPS_DONE);
                }

                // Lower the number of reps done to the maximum rep count that is selected for the exercise.
                if (set->getRepsDoneCount() > repCount) {
                    set->setRepsDoneCount(repCount);
                }
            }

            // First valid un-attempted set should blink to attract attention.
            if (firstSet && set->isValid() && !set->isAttempted()) {
                set->setState(WorkoutSetEntity::BLINKING_STATE);
                firstSet = false;
            }
        }
    }
    endResetModel();
}

WorkoutEntity* WorkoutModel::fetchWorkoutDataFromDB()
{
    QSqlQuery workoutQuery;
    bool result = workoutQuery.prepare(
                  "SELECT wt.name AS workout_name, "
                  "uw.id_workout_template, \
                  uw.day, uw.date_started, uw.date_ended, uw.last_updated, uw.user_weight, uw.completed \
                  FROM workout_template wt \
                  INNER JOIN user_workout uw ON uw.id_workout_template = wt.id_workout_template \
                  WHERE uw.id_workout = :id_workout");

    if (!result) {
        qDebug() << "Error preparing query for loading workout data.";
        qDebug() << workoutQuery.lastError();
        return 0;
    }

    workoutQuery.bindValue(":id_workout", workoutId);
    result = workoutQuery.exec();
    if (!result) {
        qDebug() << "Error getting workout data.";
        qDebug() << workoutQuery.lastError();
        return 0;
    }

    QSqlQuery query;
    result = query.prepare("SELECT wt.name AS workout_name, e.name AS exercise_name, "
                   " /* Prioritize getting current workout weight, in case we are updating an existing workout, other wise take previous workout weight, or "
                   "    fallback to NULL, we will assign default weight in app code. */ "
                  "COALESCE(uwew2.weight, previous_workout.weight) AS work_weight, "
                  "e.default_weight, "
                  "e.default_weight_increment, "
                  "COALESCE(sr2.set_count, previous_workout_sr.set_count, sr.set_count) AS set_count, "
                  "COALESCE(sr2.consistent_rep_count, previous_workout_sr.consistent_rep_count, sr.consistent_rep_count) AS rep_count, "
                  "e.id_exercise, wte.delta, uw.id_workout_template, \
                  uw.day, uw.date_started, uw.date_ended, uw.last_updated, uw.user_weight, uw.completed, e.exercise_type, \
                  COALESCE(uwew2.id_set_and_rep, previous_workout.id_set_and_rep, sr.id_set_and_rep) AS id_set_and_rep, \
                  COALESCE(uwew2.successful, 0) AS successful, \
                  COALESCE(uwew2.completed_all_sets, 0) AS completed_all_sets, \
                  COALESCE(uwew2.completed_set_count, 0) AS completed_set_count, \
                  COALESCE(previous_workout.successful, 0) AS previous_workout_successful, \
                  e.default_weight_imperial \
                  FROM workout_template wt \
                  INNER JOIN workout_template_exercises wte ON wte.id_workout_template = wt.id_workout_template \
                  INNER JOIN exercise e ON e.id_exercise = wte.id_exercise \
                  INNER JOIN set_and_rep sr ON sr.id_set_and_rep = e.id_default_set_and_rep \
                  INNER JOIN user_workout uw ON uw.id_workout_template = wt.id_workout_template \
                  LEFT JOIN user_workout_exercise_general_stats uwew2 ON uwew2.id_exercise = e.id_exercise AND uwew2.delta = wte.delta AND uwew2.id_workout = uw.id_workout \
                  LEFT JOIN set_and_rep sr2 ON sr2.id_set_and_rep = uwew2.id_set_and_rep \
                  LEFT JOIN \
                    /* Extracts a list of exercises, combined from previous workouts relative to this one, to use as a source for weights, set and rep ID, and successful flag. */ \
                    (SELECT uwew3.id_exercise, uwew3.delta, MAX(uw2.date_started), uwew3.weight, uwew3.successful, uwew3.id_set_and_rep \
                    FROM user_workout uw1 \
                    INNER JOIN user_workout uw2 ON uw2.date_started < uw1.date_started AND uw2.id_workout_template = uw1.id_workout_template AND uw2.id_user = uw1.id_user \
                    INNER JOIN user_workout_exercise_general_stats uwew3 ON uwew3.id_workout = uw2.id_workout \
                    WHERE uw1.id_workout = :id_workout \
                    GROUP BY uwew3.id_exercise, uwew3.delta \
                    ORDER BY uw2.date_started DESC) previous_workout ON previous_workout.id_exercise = e.id_exercise AND previous_workout.delta = wte.delta \
                  LEFT JOIN set_and_rep previous_workout_sr ON previous_workout_sr.id_set_and_rep = previous_workout.id_set_and_rep \
                  WHERE uw.id_workout = :id_workout AND wte.day = :workout_day \
                  ORDER BY wte.delta");
    if (!result) {
        qDebug() << "Error preparing query for loading workout exercise data.";
        qDebug() << query.lastError();
        return 0;
    }

    query.bindValue(":id_workout", workoutId);
    query.bindValue(":workout_day", workoutDay);
    result = query.exec();
    if (!result) {
        qDebug() << "Error getting workout exercise data.";
        qDebug() << query.lastError();
        return 0;
    }

    QSqlQuery queryStats;
    queryStats.prepare("SELECT id_exercise, delta, set_number, reps_done "
                       "FROM user_workout_exercise_set_stats "
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
    bool firstSet = true;

    while (workoutQuery.next()) {
        workout->name = workoutQuery.value(0).toString();
        workout->workoutTemplateId = workoutQuery.value(1).toInt();
        workout->day = workoutQuery.value(2).toInt();
        workout->dateStarted = workoutQuery.value(3).toDateTime();
        workout->dateEnded = workoutQuery.value(4).toDateTime();

        // Force save if workout has an ending date, meaning it was saved some time ago.
        // We do this, so that in case the workout was saved with no stats entered, it doesn't
        // get removed when going back a page.
        if (workout->dateEnded.isValid()) {
            workout->forceSave = true;
        }
        workout->lastUpdated = workoutQuery.value(5).toDateTime();
        workout->userWeight = workoutQuery.value(6).toReal();
        workout->completed = workoutQuery.value(7).toBool();
    }

    while (query.next()) {
        WorkoutExerciseEntity* exercise = new WorkoutExerciseEntity();
        exercise->name = query.value(1).toString();
        exercise->defaultWeight = query.value(3).toReal();
        exercise->defaultWeightImperial = query.value(22).toReal();
        exercise->defaultWeightIncrement = query.value(4).toReal();
        if (!query.isNull(2)) {
            exercise->workWeight = query.value(2).toReal();
        }
        else {
            // Assign default weight, either metric or imperial.
            User::WeightSystem system = AppState::getInstance()->getCurrentUser()->getWeightSystem();
            if (system == User::Metric) exercise->workWeight = exercise->defaultWeight;
            else exercise->workWeight = AppState::getInstance()->getWeightTransformed(exercise->defaultWeightImperial, User::Imperial, User::Metric);
        }

        // Increment weight for exercise, only if previous workout exercise was successful, and auto-add weight option was enabled.
        bool previousWorkoutExerciseSuccesful = query.value(21).toBool();
        if (previousWorkoutExerciseSuccesful && AppState::getInstance()->getCurrentUser()->getAutoAddWeight()) {
            exercise->workWeight += exercise->defaultWeightIncrement;
        }

        exercise->idExercise = query.value(7).toInt();
        exercise->delta = query.value(8).toInt();
        exercise->exerciseType = query.value(16).toInt();
        exercise->idSetAndRep = query.value(17).toInt();
        exercise->successful = query.value(18).toBool();
        exercise->completedAllSets = query.value(19).toBool();
        exercise->completedSetCount = query.value(20).toInt();

        // Display at least 5 set circles. Show crossed sets
        // in case if the sets should not be completed.
        qint32 setCount = query.value(5).toInt();
        exercise->validSetCount = setCount;
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

GenericTreeNode* WorkoutModel::parseEntityToTree(WorkoutEntity* entity) {
    if (!entity) return 0;
    GenericTreeNode* top = new GenericTreeNode(QVariant::fromValue(entity));
    for (int i = 0; i < entity->exercises.count(); ++i) {
        WorkoutExerciseEntity* exercise = entity->exercises.value(i);
        GenericTreeNode* exerciseNode = new GenericTreeNode(QVariant::fromValue(exercise), top);
        for (int j = 0; j < exercise->setsAndReps.count(); j++) {
            WorkoutSetEntity* set = exercise->setsAndReps.value(j);
            GenericTreeNode* setNode = new GenericTreeNode(QVariant::fromValue(set), exerciseNode);
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

void WorkoutModel::changeAndSaveStartDate(QDate date) {
    QSqlQuery query;
    bool result;
    query.prepare("UPDATE user_workout "
                  "SET date_started = :date_started, last_updated = :last_updated "
                  "WHERE id_workout = :id_workout");

    QDateTime currentDateTime = QDateTime::currentDateTimeUtc();
    QTime time(12, 0, 0, 0);
    QDateTime dateTime(date, time, Qt::UTC);
    query.bindValue(":date_started", dateTime);
    query.bindValue(":last_updated", currentDateTime);
    query.bindValue(":id_workout", workoutId);

    result = query.exec();
    if (!result) {
        qDebug() << "Error updating start date for workout";
        qDebug() << query.lastError();
        return;
    }
    else {
        qDebug() << "Date changed";
        workoutEntity->setDateStarted(dateTime);
    }
}

void WorkoutModel::saveWorkoutData(bool setCompleted)
{
    DBTransaction transaction;
    QSqlQuery query;
    bool result;

    // Update the workout to being completed, and set the appropriate dates, and user weight.
    if (!workoutEntity->completed && setCompleted) {
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
    // Just update the last updated date, and user weight.
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

    // Only do the following when saving a new non-completed workout, not when updating an existing one.
    if (!workoutEntity->completed && setCompleted) {
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

        qint64 newWorkoutDay = user->getNextWorkoutTemplateDay();
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

    bool isPartiallyCompleted = workoutIsPartiallyCompleted(workoutId);

    // Remove old stats data.
    query.prepare("DELETE FROM user_workout_exercise_set_stats "
                  "WHERE id_workout = :id_workout");
    query.bindValue(":id_workout", workoutId);

    result = query.exec();
    if (!result) {
        qDebug() << "Error removing old workout exercise stats data.";
        qDebug() << query.lastError();
        return;
    }

    // Save exercise stats.
    for (int i = 0; i < workoutEntity->exercises.count(); ++i) {
        WorkoutExerciseEntity* exercise = workoutEntity->exercises.value(i);

        if (!isPartiallyCompleted) {
            query.prepare("INSERT INTO user_workout_exercise_general_stats "
                          "(id_workout, id_exercise, delta, weight, id_set_and_rep, successful, completed_all_sets, completed_set_count) "
                          "VALUES (:id_workout, :id_exercise, :delta, :weight, :id_set_and_rep, :successful, :completed_all_sets, :completed_set_count)");
            query.bindValue(":id_workout", workoutId);
            query.bindValue(":id_exercise", exercise->idExercise);
            query.bindValue(":delta", exercise->delta);
            query.bindValue(":weight", exercise->workWeight);
            query.bindValue(":id_set_and_rep", exercise->idSetAndRep);
            query.bindValue(":successful", exercise->successful);
            query.bindValue(":completed_all_sets", exercise->completedAllSets);
            query.bindValue(":completed_set_count", exercise->completedSetCount);


            bool result = query.exec();
            if (!result) {
                qDebug() << "Error saving workout exercise weight data.";
                qDebug() << query.lastError();
                return;
            }
        } else {
            query.prepare("UPDATE user_workout_exercise_general_stats "
                          "SET weight = :weight, id_set_and_rep = :id_set_and_rep, successful = :successful, "
                          "completed_all_sets = :completed_all_sets, completed_set_count = :completed_set_count "
                          "WHERE id_workout = :id_workout AND id_exercise = :id_exercise AND delta = :delta");
            query.bindValue(":id_workout", workoutId);
            query.bindValue(":id_exercise", exercise->idExercise);
            query.bindValue(":delta", exercise->delta);
            query.bindValue(":weight", exercise->workWeight);
            query.bindValue(":id_set_and_rep", exercise->idSetAndRep);
            query.bindValue(":successful", exercise->successful);
            query.bindValue(":completed_all_sets", exercise->completedAllSets);
            query.bindValue(":completed_set_count", exercise->completedSetCount);

            bool result = query.exec();
            if (!result) {
                qDebug() << "Error saving workout exercise weight data.";
                qDebug() << query.lastError();
                return;
            }
        }

        // @TODO When exercise addition / removal will be implemented, this bit of code here will have to be adjusted quite a bit.
        for (int j = 0; j < exercise->setsAndReps.count(); j++) {
            WorkoutSetEntity* set = exercise->setsAndReps.value(j);

            // If next set is an invalid one (crossed for example, don't save it, nor the next ones).
            if (set->isInvalid()) break;

            query.prepare("INSERT INTO user_workout_exercise_set_stats (id_workout, id_exercise, delta, set_number, reps_done)"
                          "VALUES (:id_workout, :id_exercise, :delta, :set_number, :reps_done)");
            query.bindValue(":id_workout", workoutId);
            query.bindValue(":id_exercise", exercise->idExercise);
            query.bindValue(":delta", exercise->delta);
            query.bindValue(":set_number", set->getSetId());
            query.bindValue(":reps_done", set->getRepsDoneCount());

            result = query.exec();
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

    query.prepare("DELETE FROM user_workout_exercise_general_stats "
                  "WHERE id_workout = :id_workout");

    query.bindValue(":id_workout", workoutId);

    result = query.exec();
    if (!result) {
        qDebug() << "Error deleting workout weight stats.";
        qDebug() << query.lastError();
        return;
    }

    query.prepare("DELETE FROM user_workout_exercise_set_stats "
                  "WHERE id_workout = :id_workout");

    query.bindValue(":id_workout", workoutId);

    result = query.exec();
    if (!result) {
        qDebug() << "Error deleting workout stats.";
        qDebug() << query.lastError();
        return;
    }
}
