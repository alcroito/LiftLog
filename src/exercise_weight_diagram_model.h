#ifndef EXERCISEWEIGHTDIAGRAMMODEL_H
#define EXERCISEWEIGHTDIAGRAMMODEL_H

#include <QObject>
#include <QAbstractListModel>

class ExercisePlate {
public:
    ExercisePlate() {}
    ExercisePlate(qreal newWeight) : weight(newWeight) {}
    ExercisePlate(qreal newWeight, qreal newImperialWeight) : weight(newWeight), imperialWeight(newImperialWeight) {}
    qreal weight;
    qreal imperialWeight;
    qreal getWeight(int system);
};

class ExerciseWeightDiagramModel : public QAbstractListModel
{
    Q_OBJECT
    Q_ENUMS(PlateRoles)
public:
    enum PlateRoles {
        WeightRole = Qt::UserRole + 1,
    };

    explicit ExerciseWeightDiagramModel(QObject *parent = 0);
    virtual ~ExerciseWeightDiagramModel();

    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

    void initPlateList(qreal newWeight);
public slots:
    void switchWeight(qreal newWeight);
    QString getPlatesBreakdownText();

protected:
    QHash<int, QByteArray> roleNames() const;

private:
    QList<ExercisePlate> plateList;
    QList<ExercisePlate> availablePlates;
    qreal weight;
};

#endif // EXERCISEWEIGHTDIAGRAMMODEL_H
