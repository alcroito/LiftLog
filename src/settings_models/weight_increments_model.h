#ifndef WEIGHTINCREMENTSMODEL_H
#define WEIGHTINCREMENTSMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include "settings_interface.h"
#include "../exercise_weight_increment.h"

class WeightIncrementsModel : public QAbstractListModel, public SettingsInterface
{
    Q_OBJECT
    Q_ENUMS(WeightIncrementsModelRoles)
    Q_INTERFACES(SettingsInterface)
public:
    enum WeightIncrementsModelRoles {
        WeightIncrementWeightRole = Qt::UserRole + 1,
        WeightIncrementMetricWeightRole,
        WeightIncrementImperialWeightRole,
        WeightIncrementExerciseNameRole
    };

    explicit WeightIncrementsModel(QObject *parent = 0);

    void init();

    QVariant getSettingsProperties(const QModelIndex &index) const;
    QVariant getSettingsCellType(const QModelIndex &index) const;
    QVariant getSettingsSection(const QModelIndex &index) const;
    void refresh();
    void cellIncrementChanged(int row, bool increase);
    void resetToDefaults();


public slots:
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex &index) const;

    QList<ExerciseWeightIncrement> getWeightIncrementsFromDB();
    QList<ExerciseWeightIncrement> getDefaultIncrements();
    bool saveIncrement(ExerciseWeightIncrement &i);
    void resetUserIncrements();
protected:
     QHash<int, QByteArray> roleNames() const;

signals:
    void switchToSettingsPage(QString pageId);

private:
    QList<ExerciseWeightIncrement> incrementList;
};
#endif // WEIGHTINCREMENTSMODEL_H
