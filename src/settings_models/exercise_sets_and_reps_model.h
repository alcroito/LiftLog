#ifndef EXERCISESETSANDREPSMODEL_H
#define EXERCISESETSANDREPSMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include "settings_interface.h"

class WorkoutExerciseEntity;

class ExerciseSetsAndRepsModel : public QAbstractListModel, public SettingsInterface
{
    Q_OBJECT
    Q_ENUMS(ExerciseSetsAndRepsModelRoles)
    Q_INTERFACES(SettingsInterface)
public:
    enum ExerciseSetsAndRepsModelRoles {
        ExerciseSetsAndRepsIDRole = Qt::UserRole + 1,
        ExerciseSetsAndRepsNameRole
    };
    explicit ExerciseSetsAndRepsModel(QObject *parent = 0);
    ~ExerciseSetsAndRepsModel();

    void init();
    void clear();

    QVariant getSettingsProperties(const QModelIndex &index) const;
    QVariant getSettingsCellType(const QModelIndex &index) const;
    QVariant getSettingsSection(const QModelIndex &index) const;
    void refresh();


public slots:
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

    QList<WorkoutExerciseEntity*> getExercisesFromDB();
protected:
     QHash<int, QByteArray> roleNames() const;

signals:
    void switchToSettingsPage(QString pageId);

private:
    QList<WorkoutExerciseEntity*> exerciseList;
};

#endif // EXERCISESETSANDREPSMODEL_H
