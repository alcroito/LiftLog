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
    ExerciseStatPoint(qreal newWeight, QDateTime newDate, QString newRepsDone) : weight(newWeight), date(newDate), repsDone(newRepsDone) {}
    qint64 timestamp() { return date.toMSecsSinceEpoch() / 1000; }
    QString getRepsDone() { return repsDone; }
    qreal weight;
    QDateTime date;
    QString repsDone;
};

class ExerciseStatsData {
public:
    qint64 idExercise;
    QString name;
    QColor color;
    QList<ExerciseStatPoint> points;
    void clear() { points.clear(); }
    void sortPointsByTimeStamp();
};

class StatsGraphData : public QObject
{
    Q_OBJECT
    Q_ENUMS(DatePeriod)
    Q_ENUMS(DataToShow)
    Q_PROPERTY(DatePeriod period READ getPeriod WRITE setPeriod NOTIFY periodChanged)
    Q_PROPERTY(DataToShow dataToShow READ getDataToShow WRITE setDataToShow NOTIFY dataToShowChanged)
    Q_PROPERTY(qint64 idExerciseToShow READ getIdExerciseToShow WRITE setIdExerciseToShow NOTIFY idExerciseToShowChanged)
    Q_PROPERTY(qint32 listIndex READ getListIndex WRITE setListIndex NOTIFY listIndexChanged)
public:
    explicit StatsGraphData(QObject *parent = 0);

    ExerciseStatPoint& getPointForExerciseIndex(qint32 exerciseIndex, qint32 pointIndex);

    QColor getColorForIndex(qint32 index);
    qreal mapToRange(qint64 inputStart, qint64 inputEnd, qint64 outputStart, qint64 outputEnd, qint64 value);
    QVector2D normalizeToUnitVector(QVector2D p);
    void printAllPoints();

    enum DatePeriod {
        All = 0,
        OneMonth,
        ThreeMonths,
        SixMonths,
        Specific
    };

    enum DataToShow {
        AllExercisesAndBodyWeight = 0,
        SpecificExercise,
        BodyWeight,
    };

    class StatsQueryParams {
    public:
        DatePeriod datePeriod = All;
        QDateTime specificDate = QDateTime();
        DataToShow dataToShow = AllExercisesAndBodyWeight;
        qint64 idExerciseToShow = 0;
        QString nameOfExerciseToShow;
    };

    const qint64 fakeBodyWeightExerciseId = -2;

public slots:
    DatePeriod getPeriod() const;
    void setPeriod(DatePeriod newDatePeriod);

    qint64 getIdExerciseToShow() const;
    void setIdExerciseToShow(qint64 value);

    DataToShow getDataToShow() const;
    void setDataToShow(DataToShow value);

    qint32 getListIndex() const;
    void setListIndex(const qint32 &value);

    void getStatsFromDB();
    qint32 exerciseCount();
    qint32 pointCount(qint32 exerciseIndex);

    QString exerciseName(qint32 exerciseIndex);
    QColor exerciseColor(qint32 exerciseIndex);

    qreal weightForExercisePoint(qint32 exerciseIndex, qint32 pointIndex);
    QDateTime dateForExercisePoint(qint32 exerciseIndex, qint32 pointIndex);
    qint64 timestampForExercisePoint(qint32 exerciseIndex, qint32 pointIndex);
    QString repsDoneForExercisePoint(qint32 exerciseIndex, qint32 pointIndex);

    QVariantMap getLowerAndUpperBounds();
    QPoint getCoordinatePoint(qint32 exerciseIndex, qint32 pointIndex);
    qint32 getBestSegmentCount();
    QVariantMap getNearestPointAndExerciseData(QPoint p, qint32 exerciseIndex = -1);

    static qint32 getMinPointCountForAnyExerciseFromDB();
    static QSqlQuery runStatsQuery(const StatsQueryParams &params, bool* ok = 0);
    static QDateTime getLatestDateWithMinimumRequiredExercisePoints(bool *ok = 0);
    static QList<StatsQueryParams> getListOfExerciseQueryParamsThatHaveEnoughPoints();
signals:
    void periodChanged(DatePeriod);
    void idExerciseToShowChanged(qint64);
    void dataToShowChanged(DataToShow);
    void listIndexChanged(qint32);
private:
    QList<ExerciseStatsData> exercises;
    qint32 maxExercisePointCount;
    QVariantMap bounds;
    DatePeriod period;
    DataToShow dataToShow;
    qint64 idExerciseToShow;
    qint32 listIndex;
};

class StatsGraphDataSingleton : public QObject {
    Q_OBJECT
public:
    explicit StatsGraphDataSingleton(QObject *parent = 0);
public slots:
    qint32 getMinPointCountForAnyExerciseFromDB();
};

QDebug operator<<(QDebug dbg, ExerciseStatPoint& p);
QDebug operator<<(QDebug dbg, const StatsGraphData::StatsQueryParams& p);

#endif // STATSGRAPHDATA_H
