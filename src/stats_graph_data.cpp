#include "stats_graph_data.h"
#include "app_state.h"
#include "user.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <limits>

StatsGraphData::StatsGraphData(QObject *parent) : QObject(parent)
{
    getStatsFromDB();
}

void StatsGraphData::getStatsFromDB()
{
    exercises.clear();

    QSqlQuery query;
    QString queryString = "SELECT e.name, e.id_exercise, uwew.weight, sr.set_count, sr.rep_count, uw.id_workout, uw.date_started, e.tags \
            FROM user_workout uw \
            INNER JOIN workout_template_exercises wte ON wte.id_workout_template = uw.id_workout_template AND wte.day = uw.day \
            INNER JOIN exercise e ON e.id_exercise = wte.id_exercise \
            INNER JOIN user_workout_exercise_weight uwew ON uwew.id_workout = uw.id_workout AND wte.id_exercise = uwew.id_exercise \
            INNER JOIN set_and_rep sr ON sr.id_set_and_rep = uwew.id_set_and_rep \
            WHERE uw.id_user = :id_user AND uw.id_workout_template = :id_workout_template \
            ORDER BY wte.id_exercise, date_started ASC";
    query.prepare(queryString);

    User* user = AppState::getInstance()->getCurrentUser();
    qint64 userId = user->getId();
    query.bindValue(":id_user", userId);
    query.bindValue(":id_workout_template", user->getLastIdWorkoutTemplate());

    bool result = query.exec();
    if (!result) {
        qDebug() << "Error getting graph stats data.";
        qDebug() << query.lastError();
        return;
    }

    qint64 prevExerciseId = 0;

    ExerciseStatsData statsData;

    int count = 0;
    int exerciseIndex = 0;
    int exercisePointCount = 0;
    while (query.next()) {
        exercisePointCount++;
        QString name = query.value(0).toString();
        qint64 idExercise = query.value(1).toInt();
        qreal weight = query.value(2).toReal();
        qint32 setCount = query.value(3).toInt();
        qint32 repCount = query.value(4).toInt();
        qint64 idWorkout = query.value(5).toInt();
        Q_UNUSED(idWorkout);
        QDateTime date = query.value(6).toDateTime();
        QString tags = query.value("tags").toString();

        // Skip accessory exercises for now.
        bool isAccessory = tags.contains("accessory");
        if (isAccessory) continue;

        if (idExercise != prevExerciseId) {
            if (prevExerciseId != 0) {
                exercises.append(statsData);
            }

            statsData.clear();
            statsData.idExercise = idExercise;
            statsData.name = name;
            statsData.setsAndReps = QString("%1x%2").arg(setCount).arg(repCount);
            statsData.color = getColorForIndex(exerciseIndex);
            exerciseIndex++;

            if (exercisePointCount > maxExercisePointCount) {
                maxExercisePointCount = exercisePointCount;
            }
            exercisePointCount = 0;
        }

        ExerciseStatPoint point(weight, date);
        statsData.points.append(point);

        prevExerciseId = idExercise;
        count++;

    }

    if (count > 0) {
        exercises.append(statsData);
    }
}

qint32 StatsGraphData::getBestSegmentCount() {
    if (maxExercisePointCount < 6) {
        return 4;
    } else {
        return 5;
    }
}

QVariantMap StatsGraphData::getLowerAndUpperBounds() {
    qint64 xLower = std::numeric_limits<qint64>::max(), xUpper = std::numeric_limits<qint64>::min();
    qint64 yLower = std::numeric_limits<qint64>::max(), yUpper = std::numeric_limits<qint64>::min();
    for (int i = 0; i < exerciseCount(); i++) {
        for (int j = 0; j < pointCount(i); j++) {
            ExerciseStatPoint point = getPointForExerciseIndex(i, j);

            qint64 timestamp = point.timestamp();
            if (timestamp < xLower) {
                xLower = timestamp;
            }
            if (timestamp > xUpper) {
                xUpper = timestamp;
            }

            if (point.weight < yLower) {
                yLower = point.weight;
            }
            if (point.weight > yUpper) {
                yUpper = point.weight;
            }
        }
    }

    if (xLower == xUpper) {
        qint64 middle = xLower;
        qint64 day = 60 * 60 * 24;
        xLower = middle - 3 * day;
        xUpper = middle + 3 * day;
    }

    QVariantMap result;
    result.insert("xLower", xLower);
    result.insert("xUpper", xUpper);
    result.insert("yLower", yLower);
    result.insert("yUpper", yUpper);
    return result;
}

QColor StatsGraphData::getColorForIndex(qint32 index) {
    QColor color("#e1352d");
    switch (index) {
        case 0: color = "#8b488d"; // violet
            break;
        case 1: color = "#f89446"; // orange
            break;
        case 2: color = "#276aa0"; // ocean blue
            break;
        case 3: color = "#8f9291"; // steel gray
            break;
        case 4: color = "#83c07d"; // salad green
            break;
        case 5: color = "#e1352d"; // red
            break;
        case 6: color = "#ffff99"; // sunny yellow
            break;
    }
    return color;
}

qint32 StatsGraphData::exerciseCount() { return exercises.count(); }

ExerciseStatPoint& StatsGraphData::getPointForExerciseIndex(qint32 exerciseIndex, qint32 pointIndex) {
    return exercises[exerciseIndex].points[pointIndex];
}

QPoint StatsGraphData::getCoordinatePoint(qint32 exerciseIndex, qint32 pointIndex) {
    ExerciseStatPoint point = getPointForExerciseIndex(exerciseIndex, pointIndex);
    return QPoint(point.timestamp(), point.weight);
}

qint32 StatsGraphData::pointCount(qint32 exerciseIndex)
{
    return exercises[exerciseIndex].points.count();
}

QString StatsGraphData::exerciseName(qint32 exerciseIndex)
{
    return exercises[exerciseIndex].name;
}

QString StatsGraphData::exerciseSetsAndReps(qint32 exerciseIndex)
{
    return exercises[exerciseIndex].setsAndReps;
}

QColor StatsGraphData::exerciseColor(qint32 exerciseIndex)
{
    return exercises[exerciseIndex].color;
}

qreal StatsGraphData::weightForExercisePoint(qint32 exerciseIndex, qint32 pointIndex)
{
    return exercises[exerciseIndex].points[pointIndex].weight;
}

QDateTime StatsGraphData::dateForExercisePoint(qint32 exerciseIndex, qint32 pointIndex)
{
    return exercises[exerciseIndex].points[pointIndex].date;
}

qint64 StatsGraphData::timestampForExercisePoint(qint32 exerciseIndex, qint32 pointIndex)
{
    return exercises[exerciseIndex].points[pointIndex].date.toMSecsSinceEpoch();
}

