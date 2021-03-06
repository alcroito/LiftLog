#include "stats_graph_data.h"
#include "app_state.h"
#include "user.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QVector2D>
#include <QSet>
#include <limits>
#include <cmath>

StatsGraphData::StatsGraphData(QObject *parent) :
    QObject(parent), maxExercisePointCount(std::numeric_limits<decltype(maxExercisePointCount)>::min()), period(All), dataToShow(AllExercisesAndBodyWeight),
    idExerciseToShow(0), listIndex(0)
{

}

QSqlQuery StatsGraphData::runStatsQuery(const StatsQueryParams& params, bool* ok) {
    QSqlQuery query;
    QString queryString = "SELECT e.name, e.id_exercise, uwew.weight, sr.set_count, sr.consistent_rep_count AS rep_count, \
                          uw.id_workout, uw.date_started, e.exercise_type, GROUP_CONCAT(uwes.reps_done, '/') AS reps_done, uwew.successful, uw.user_weight \
            FROM user_workout uw \
            INNER JOIN workout_template_exercises wte ON wte.id_workout_template = uw.id_workout_template AND wte.day = uw.day \
            INNER JOIN exercise e ON e.id_exercise = wte.id_exercise \
            INNER JOIN user_workout_exercise_general_stats uwew ON uwew.id_workout = uw.id_workout AND wte.id_exercise = uwew.id_exercise \
            INNER JOIN user_workout_exercise_set_stats uwes ON uwes.id_workout = uw.id_workout AND uwes.id_exercise = uwew.id_exercise AND uwes.delta = uwew.delta \
            INNER JOIN set_and_rep sr ON sr.id_set_and_rep = uwew.id_set_and_rep \
            WHERE uw.id_user = :id_user AND uw.id_workout_template = :id_workout_template AND uw.completed = 1 %1 %2 \
            GROUP BY e.name, e.id_exercise, uwew.weight, sr.set_count, sr.consistent_rep_count, uw.id_workout, uw.date_started, e.exercise_type \
            ORDER BY wte.id_exercise, date_started, uwes.set_number ASC";
    if (params.datePeriod == All) {
        queryString = queryString.arg("");
    } else {
        QDateTime finalDate;
        if (params.datePeriod == Specific) {
            finalDate = params.specificDate;
        } else {
            QDateTime date = QDateTime::currentDateTimeUtc();
            if (params.datePeriod == OneMonth) date = date.addMonths(-1);
            else if (params.datePeriod == ThreeMonths) date = date.addMonths(-3);
            else if (params.datePeriod == SixMonths) date = date.addMonths(-6);
            finalDate = date;
        }
        queryString = queryString.arg(" AND uw.date_started > '%1'").arg(finalDate.toString("yyyy-MM-ddTHH:mm:ss.z"));
    }

    if (params.dataToShow == AllExercisesAndBodyWeight || params.dataToShow == BodyWeight) {
        queryString = queryString.arg("");
    } else {
        queryString = queryString.arg(" AND e.id_exercise = %2").arg(params.idExerciseToShow);

    }

    bool prepareResult = query.prepare(queryString);
    if (ok) *ok = prepareResult;
    if (!prepareResult) {
        qDebug() << "Error getting graph stats data.";
        qDebug() << query.lastError();
        return query;
    }

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

QDateTime StatsGraphData::getLatestDateWithMinimumRequiredExercisePoints(bool* ok) {
    QDateTime date;
    QSqlQuery query;
    QString queryString = " \
            SELECT date_started, COUNT(id_exercise) AS exercise_count_with_minimum_required_points \
            FROM \
            ( \
                SELECT a.date_started, e.id_exercise, COUNT(e.id_exercise) AS point_count \
                FROM user_workout uw \
                INNER JOIN workout_template_exercises wte ON wte.id_workout_template = uw.id_workout_template AND wte.day = uw.day \
                INNER JOIN exercise e ON e.id_exercise = wte.id_exercise \
                INNER JOIN user_workout_exercise_general_stats uwew ON uwew.id_workout = uw.id_workout AND wte.id_exercise = uwew.id_exercise \
                INNER JOIN set_and_rep sr ON sr.id_set_and_rep = uwew.id_set_and_rep \
                INNER JOIN (SELECT DISTINCT uw2.date_started FROM user_workout uw2) a ON uw.date_started >= a.date_started \
                WHERE uw.id_user = :id_user AND uw.id_workout_template = :id_workout_template AND uw.completed = 1 \
                GROUP BY a.date_started, e.id_exercise \
                HAVING COUNT(e.id_exercise) >= 2 \
            ) \
            GROUP BY date_started \
            ORDER BY exercise_count_with_minimum_required_points ASC \
            LIMIT 1";

    query.prepare(queryString);

    User* user = AppState::getInstance()->getCurrentUser();
    qint64 userId = user->getId();
    query.bindValue(":id_user", userId);
    query.bindValue(":id_workout_template", user->getLastIdWorkoutTemplate());

    if (ok) *ok = false;
    bool result = query.exec();
    if (!result) {
        qDebug() << "Error getting latest date with minimum required exercise points for graph page.";
        qDebug() << query.lastError();

        return date;
    }

    while (query.next()) {
        date = query.value("date_started").toDateTime();
        if (ok) *ok = true;
        break;
    }

    return date;
}

QList<StatsGraphData::StatsQueryParams> StatsGraphData::getListOfExerciseQueryParamsThatHaveEnoughPoints()
{
    StatsQueryParams initialParams;
    QList<StatsGraphData::StatsQueryParams> list;
    QSet<qint64> exerciseIdsSeen;
    bool ok = false;

    QSqlQuery query = runStatsQuery(initialParams, &ok);
    if (!ok) {
        qWarning() << "Error getting list of exercise query parameters that have enough points for the graph.";
        return list;
    }

    int exerciseCountWithEnoughPoints = 0;
    while (query.next()) {
        QString name = query.value("name").toString();
        qint64 exerciseType = query.value("exercise_type").toInt();
        bool isAccessory = exerciseType == 1;
        if (isAccessory) continue;

        qint64 idExercise = query.value("id_exercise").toInt();
        if (exerciseIdsSeen.contains(idExercise)) continue;
        exerciseIdsSeen.insert(idExercise);

        StatsQueryParams params;
        params.idExerciseToShow = idExercise;
        params.dataToShow = SpecificExercise;
        params.nameOfExerciseToShow = name;
        list.append(params);

        exerciseCountWithEnoughPoints++;
    }

    // If there are exercises to show, we also show All and Body weight configurations.
    if (exerciseCountWithEnoughPoints > 0) {
        // Prepend body weight configuration.
        StatsQueryParams params;
        params.dataToShow = BodyWeight;
        params.nameOfExerciseToShow = tr("Body weight");
        list.prepend(params);

        // Prepend All Exercises + body weight configuration.
        params.dataToShow = AllExercisesAndBodyWeight;
        params.nameOfExerciseToShow = tr("All progress");
        list.prepend(params);
    }

    return list;
}

qint32 StatsGraphData::getMinPointCountForAnyExerciseFromDB() {
    auto count = 0;
    bool ok;
    auto exercisePointCount = 0;
    auto minExercisePointCount = 0;
    qint64 prevExerciseId = 0;

    StatsQueryParams params;
    QSqlQuery query = runStatsQuery(params, &ok);

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
    StatsQueryParams params;
    params.datePeriod = period;
    params.dataToShow = dataToShow;
    params.idExerciseToShow = idExerciseToShow;

    exercises.clear();
    bool statsFound = false;
    int statFindingTries = 0;

    QSet<qint64> workoutIdsSeen;
    ExerciseStatsData bodyWeightFakeExercise;
    bodyWeightFakeExercise.idExercise = fakeBodyWeightExerciseId;
    bodyWeightFakeExercise.name = tr("Body weight");
    bodyWeightFakeExercise.color = "#e1352d";

    while (!statsFound && statFindingTries < 4) {
        statFindingTries++;
        bool ok;
        QSqlQuery query = runStatsQuery(params, &ok);
        if (!ok) {
            return;
        }

        qint64 prevExerciseId = 0;
        ExerciseStatsData statsData;

        int totalPointCount = 0;
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
            QDateTime date = query.value(6).toDateTime();
            qint64 exerciseType = query.value("exercise_type").toInt();
            QString repsDone = query.value("reps_done").toString();

            bool exerciseSuccessful = query.value("successful").toBool();
            qreal userBodyWeight = query.value("user_weight").toReal();
            QString repsDoneFinal = QString("%1x%2").arg(setCount).arg(repCount);
            if (!exerciseSuccessful) {
                if (setCount == 1) {
                    repsDoneFinal = "1x" + repsDone;
                } else {
                    repsDoneFinal = repsDone;
                }
            }

            // Skip accessory exercises for now.
            bool isAccessory = exerciseType == 1;
            if (isAccessory) continue;

            if (idExercise != prevExerciseId) {
                if (prevExerciseId != 0) {
                    exercises.append(statsData);
                }

                statsData.clear();
                statsData.idExercise = idExercise;
                statsData.name = name;

                // If this data represents a single specific exercise, we display the color
                // that displayed on the All page.
                if (dataToShow == SpecificExercise) {
                    statsData.color = getColorForIndex(getListIndex());
                }
                // Otherwise display colors based on the order in the exercises list (All page).
                else {
                    statsData.color = getColorForIndex(exerciseIndex);
                }

                exerciseIndex++;

                if (exercisePointCount > maxExercisePointCount) {
                    maxExercisePointCount = exercisePointCount;
                }
                exercisePointCount = 0;
            }

            if (!workoutIdsSeen.contains(idWorkout)) {
                ExerciseStatPoint userWeightPoint(userBodyWeight, date, "");
                bodyWeightFakeExercise.points.append(userWeightPoint);
                workoutIdsSeen.insert(idWorkout);
            }

            ExerciseStatPoint point(weight, date, repsDoneFinal);
            statsData.points.append(point);

            prevExerciseId = idExercise;
            totalPointCount++;
        }

        if (totalPointCount > 0) {
            exercises.append(statsData);
            bodyWeightFakeExercise.sortPointsByTimeStamp();
            if (dataToShow == BodyWeight) {
                exercises.clear();
            }

            if (dataToShow != SpecificExercise) {
                exercises.append(bodyWeightFakeExercise);
            }

            if (exercisePointCount > maxExercisePointCount) {
                maxExercisePointCount = exercisePointCount;
            }
            statsFound = true;
        } else {
            // There were no points for the executed query, so we try a different query that should return results.
            params.specificDate = getLatestDateWithMinimumRequiredExercisePoints(&ok);
            if (ok) {
                period = Specific;
                params.datePeriod = Specific;
            }
        }
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
    auto max = std::numeric_limits<qint64>::max();
    auto min = std::numeric_limits<qint64>::min();
    qint64 xLower = max, xUpper = min;
    qint64 yLower = max, yUpper = min;
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

    // Take care of case when no bounds are set, because no exercises are present.
    // That should not happen, but just in case I missed something.

    bool boundsSet = true;
    if (xLower == max || xUpper == min || yLower == max || yUpper == min) {
        boundsSet = false;
    }

    // Take care of degenerate case when the range of values is 0.
    if (xLower == xUpper) {
        qint64 middle = xLower;
        qint64 day = 60 * 60 * 24;
        xLower = middle - 3 * day;
        xUpper = middle + 3 * day;
    }

    // Take care of degenerate case when the range of values is 0.
    if (yLower == yUpper) {
        qint64 middle = yLower;
        qint64 carefullyChosenDeltaThatIsNotActuallySo = 10;
        yLower = middle - carefullyChosenDeltaThatIsNotActuallySo;
        yUpper = middle + carefullyChosenDeltaThatIsNotActuallySo;
    }

    QVariantMap result;
    result.insert("xLower", xLower);
    result.insert("xUpper", xUpper);
    result.insert("yLower", yLower);
    result.insert("yUpper", yUpper);
    result.insert("boundsSet", boundsSet);

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

StatsGraphData::DatePeriod StatsGraphData::getPeriod() const { return period; }

void StatsGraphData::setPeriod(StatsGraphData::DatePeriod newDatePeriod) {
    if (newDatePeriod != period) {
        period = newDatePeriod;
        emit periodChanged(period);
    }
}

qint64 StatsGraphData::getIdExerciseToShow() const
{
    return idExerciseToShow;
}

void StatsGraphData::setIdExerciseToShow(qint64 value)
{
    if (value != idExerciseToShow) {
        idExerciseToShow = value;
        emit idExerciseToShowChanged(value);
    }
}

StatsGraphData::DataToShow StatsGraphData::getDataToShow() const
{
    return dataToShow;
}

void StatsGraphData::setDataToShow(StatsGraphData::DataToShow value)
{
    if (value != dataToShow) {
        dataToShow = value;
        emit dataToShowChanged(value);
    }
}

qint32 StatsGraphData::getListIndex() const
{
    return listIndex;
}

void StatsGraphData::setListIndex(const qint32 &value)
{
    if (listIndex != value) {
        listIndex = value;
        emit listIndexChanged(value);
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

    // Safe-guard against any case when there was a problem selecting a point.
    if (minI == -1 || minJ == -1 || minPointI == -1 || minPointJ == -1) {
        qWarning() << "Error finding nearest point to given point";
        return QVariantMap();
    }

//    qDebug() << "the point " << p << " point distance " << smallestPointDistance << " line distance " << smallestLineDistance;

    // Get the nearest point bubble text.
    ExerciseStatsData e = exercises[minI];
    ExerciseStatPoint statPoint = getPointForExerciseIndex(minPointI, minPointJ);
    AppState* app = AppState::getInstance();
    QString weightString = app->getWeightStringBuilder(statPoint.weight)->get();
    QString bubbleText = QString("%1\n%2 %3\n%4").arg(e.name).arg(weightString).arg(statPoint.getRepsDone()).arg(statPoint.date.toString("MMM dd, yyyy"));

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

QString StatsGraphData::repsDoneForExercisePoint(qint32 exerciseIndex, qint32 pointIndex)
{
    return exercises[exerciseIndex].points[pointIndex].getRepsDone();
}

QDebug operator<<(QDebug dbg, ExerciseStatPoint &p) {
    return dbg << "Stat point" << p.weight << p.timestamp();
}

QDebug operator<<(QDebug dbg, const StatsGraphData::StatsQueryParams &p) {
    return dbg << "Stats query params " << p.dataToShow << " " << p.idExerciseToShow;
}

StatsGraphDataSingleton::StatsGraphDataSingleton(QObject *parent) : QObject(parent)
{

}

qint32 StatsGraphDataSingleton::getMinPointCountForAnyExerciseFromDB()
{
    return StatsGraphData::getMinPointCountForAnyExerciseFromDB();
}


void ExerciseStatsData::sortPointsByTimeStamp()
{
    std::sort(points.begin(), points.end(), [] (ExerciseStatPoint& a, ExerciseStatPoint& b) -> bool {
        return a.timestamp() < b.timestamp();
    });
}
