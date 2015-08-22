#include "stats_graph_data.h"
#include "app_state.h"
#include "user.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QVector2D>
#include <limits>
#include <cmath>

StatsGraphData::StatsGraphData(QObject *parent) : QObject(parent), maxExercisePointCount(std::numeric_limits<decltype(maxExercisePointCount)>::min())
{
    getStatsFromDB();
}

QSqlQuery StatsGraphData::runStatsQuery(bool* ok) {
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
    if (ok) *ok = result;
    if (!result) {
        qDebug() << "Error getting graph stats data.";
        qDebug() << query.lastError();
    }

    return query;
}

qint32 StatsGraphData::getMinPointCountForAnyExerciseFromDB() {
    auto count = 0;
    bool ok;
    auto exercisePointCount = 0;
    auto minExercisePointCount = 0;
    qint64 prevExerciseId = 0;

    QSqlQuery query = runStatsQuery(&ok);

    if (ok) {
        minExercisePointCount = std::numeric_limits<decltype(minExercisePointCount)>::max();
        while (query.next()) {
            qint64 idExercise = query.value("id_exercise").toInt();

            if (idExercise != prevExerciseId && prevExerciseId != 0) {
                if (exercisePointCount < minExercisePointCount) {
                    minExercisePointCount = exercisePointCount;
                }

                exercisePointCount = 0;
            }
            exercisePointCount++;
            prevExerciseId = idExercise;
        }

    }

    if (exercisePointCount > 0) {
        if (exercisePointCount < minExercisePointCount) {
            minExercisePointCount = exercisePointCount;
        }
        count = minExercisePointCount;
    }

    return count;
}

void StatsGraphData::getStatsFromDB()
{
    exercises.clear();

    bool ok;
    QSqlQuery query = runStatsQuery(&ok);
    if (!ok) {
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
        if (exercisePointCount > maxExercisePointCount) {
            maxExercisePointCount = exercisePointCount;
        }
    }

//    printAllPoints();
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

    bounds = result;
    return result;
}

double round(double d)
{
    return floor(d + 0.5);
}

qreal StatsGraphData::mapToRange(qint64 inputStart, qint64 inputEnd, qint64 outputStart, qint64 outputEnd, qint64 value) {
    double slope = 1.0 * (outputEnd - outputStart) / (inputEnd - inputStart);
    double output = outputStart + slope * (value - inputStart);
    return output;
}

QVector2D StatsGraphData::normalizeToUnitVector(QVector2D p) {
    p.setX(mapToRange(bounds["xLower"].toLongLong(), bounds["xUpper"].toLongLong(), 0, 1, p.x()));
    p.setY(mapToRange(bounds["yLower"].toLongLong(), bounds["yUpper"].toLongLong(), 0, 1, p.y()));
    return p;
}

void StatsGraphData::printAllPoints()
{
    for (int i = 0; i < exerciseCount(); ++i) {
        qDebug() << "exercise " << exerciseName(i);
        for (int j = 0; j < pointCount(i); j++) {
            qDebug() << getPointForExerciseIndex(i, j);
        }
    }
}

QVariantMap StatsGraphData::getNearestPointAndExerciseData(QPoint p, qint32 exerciseIndex) {
    qreal smallestLineDistance = std::numeric_limits<qreal>::max();
    qreal smallestPointDistance = std::numeric_limits<qreal>::max();

    QVector2D pNormalized = normalizeToUnitVector(QVector2D(p));

    int minI = -1, minJ = -1, minPointI = -1, minPointJ = -1, i = 0, endingCondition = exerciseCount();

    // Search only for a point within a single exercise if specified.
    if (exerciseIndex != -1) {
        i = exerciseIndex;
        endingCondition = i + 1;
    }

    for (; i < endingCondition; i++) {
        QVector2D prevPoint;

        // Add a fake initial point, for better line detection when near the left side of the graph.
        if (pointCount(i) > 0) {
            ExerciseStatPoint point = getPointForExerciseIndex(i, 0);
            prevPoint = QVector2D(point.timestamp(), point.weight);
            prevPoint = normalizeToUnitVector(prevPoint);
            prevPoint.setX(0);
        }

        // Go through each point, and find the distance to each point, and the distance to the line
        // formed by the current and previous point.
        for (int j = 0; j < pointCount(i); j++) {
            ExerciseStatPoint point = getPointForExerciseIndex(i, j);

            QVector2D vectorPoint = QVector2D(point.timestamp(), point.weight);
            vectorPoint = normalizeToUnitVector(vectorPoint);

            QVector2D direction = (vectorPoint - prevPoint).normalized();

            // We use 2 distances, the line distance is to better figure out which exercise was chosen.
            // The point distance is to figure out which point is the closest, to show it's information.
            qreal pointDistance = pNormalized.distanceToPoint(vectorPoint);
            qreal lineDistance = pNormalized.distanceToLine(vectorPoint, direction);

            prevPoint = vectorPoint;

            if (pointDistance <= smallestPointDistance) {
                smallestPointDistance = pointDistance;
                minPointI = i;
                minPointJ = j;
            }

            if (lineDistance <= smallestLineDistance) {
                smallestLineDistance = lineDistance;
                minI = i;
                minJ = j;
            }
        }
    }

//    qDebug() << "the point " << p << " point distance " << smallestPointDistance << " line distance " << smallestLineDistance;

    // Get the nearest point bubble text.
    ExerciseStatsData e = exercises[minI];
    ExerciseStatPoint statPoint = getPointForExerciseIndex(minPointI, minPointJ);
    AppState* app = AppState::getInstance();
    QString bubbleText = QString("%1\n%2 %3\n%4").arg(e.name).arg(app->getWeightString(statPoint.weight)).arg(e.setsAndReps).arg(statPoint.date.toString("MMM dd, yyyy"));

    // Arbitrary minimum distance to point, to know when to show the bubble text.
    bool closeEnoughToPoint = smallestPointDistance < 0.1;

    QVariantMap result;
    result.insert("exerciseIndex", minI);
    result.insert("pointIndex", minJ);
    result.insert("color", exerciseColor(minI));
    result.insert("bubbleText", bubbleText);
    result.insert("closeEnoughToPoint", closeEnoughToPoint);
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

QDebug &operator<<(QDebug dbg, ExerciseStatPoint &p) {
    return dbg << "stat point" << p.weight << p.timestamp();
}


StatsGraphDataSingleton::StatsGraphDataSingleton(QObject *parent) : QObject(parent)
{

}

qint32 StatsGraphDataSingleton::getMinPointCountForAnyExerciseFromDB()
{
    return StatsGraphData::getMinPointCountForAnyExerciseFromDB();
}
