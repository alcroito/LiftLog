#ifndef STATSGRAPHDATA_H
#define STATSGRAPHDATA_H

#include <QObject>
#include <QDateTime>
#include <QVariant>
#include <QPoint>
#include <QColor>
#include <QDebug>
#include <QSqlQuery>

class ExerciseStatPoint {
public:
    ExerciseStatPoint() {}
    ExerciseStatPoint(qreal newWeight, QDateTime newDate) : weight(newWeight), date(newDate) {}
    qint64 timestamp() { return date.toMSecsSinceEpoch() / 1000; }
    qreal weight;
    QDateTime date;
};

QDebug& operator<<(QDebug dbg, ExerciseStatPoint& p);

class ExerciseStatsData {
public:
    qint64 idExercise;
    QString name;
    QString setsAndReps;
    QColor color;
    QList<ExerciseStatPoint> points;
    void clear() { points.clear(); }
};

class StatsGraphData : public QObject
{
    Q_OBJECT
public:
    explicit StatsGraphData(QObject *parent = 0);

    ExerciseStatPoint& getPointForExerciseIndex(qint32 exerciseIndex, qint32 pointIndex);

    QColor getColorForIndex(qint32 index);
    qreal mapToRange(qint64 inputStart, qint64 inputEnd, qint64 outputStart, qint64 outputEnd, qint64 value);
    QVector2D normalizeToUnitVector(QVector2D p);
    void printAllPoints();
public slots:
    void getStatsFromDB();
    qint32 exerciseCount();
    qint32 pointCount(qint32 exerciseIndex);

    QString exerciseName(qint32 exerciseIndex);
    QString exerciseSetsAndReps(qint32 exerciseIndex);
    QColor exerciseColor(qint32 exerciseIndex);

    qreal weightForExercisePoint(qint32 exerciseIndex, qint32 pointIndex);
    QDateTime dateForExercisePoint(qint32 exerciseIndex, qint32 pointIndex);
    qint64 timestampForExercisePoint(qint32 exerciseIndex, qint32 pointIndex);
    QVariantMap getLowerAndUpperBounds();
    QPoint getCoordinatePoint(qint32 exerciseIndex, qint32 pointIndex);
    qint32 getBestSegmentCount();
    QVariantMap getNearestPointAndExerciseData(QPoint p, qint32 exerciseIndex = -1);
    static qint32 getMinPointCountForAnyExerciseFromDB();
    static QSqlQuery runStatsQuery(bool* ok = 0);
private:
    QList<ExerciseStatsData> exercises;
    qint32 maxExercisePointCount;
    QVariantMap bounds;
};

class StatsGraphDataSingleton : public QObject {
    Q_OBJECT
public:
    explicit StatsGraphDataSingleton(QObject *parent = 0);
public slots:
    qint32 getMinPointCountForAnyExerciseFromDB();
};

#endif // STATSGRAPHDATA_H
