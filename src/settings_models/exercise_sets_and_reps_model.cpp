#include "exercise_sets_and_reps_model.h"
#include "../workout_exercise_entity.h"
#include "../app_state.h"
#include "../user.h"
#include "../icons.h"
#include "settings_pages.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

ExerciseSetsAndRepsModel::ExerciseSetsAndRepsModel(QObject *parent) : QAbstractListModel(parent)
{

}

ExerciseSetsAndRepsModel::~ExerciseSetsAndRepsModel()
{
    clear();
}

int ExerciseSetsAndRepsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return exerciseList.count();
}

Qt::ItemFlags ExerciseSetsAndRepsModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;
    return Qt::ItemIsEditable | QAbstractItemModel::flags(index);
}

QVariant ExerciseSetsAndRepsModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= exerciseList.count())
            return QVariant();

    const WorkoutExerciseEntity* exercise = exerciseList[index.row()];
    if (role == ExerciseSetsAndRepsNameRole) {
        return exercise->name;
    }
    else if (role == ExerciseSetsAndRepsIDRole) {
        return exercise->idExercise;
    }
    return QVariant();
}

QHash<int, QByteArray> ExerciseSetsAndRepsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[ExerciseSetsAndRepsIDRole] = "idExercise";
    roles[ExerciseSetsAndRepsNameRole] = "name";
    return roles;
}

void ExerciseSetsAndRepsModel::clear() {
    qDeleteAll(exerciseList);
    exerciseList.clear();
}

QList<WorkoutExerciseEntity*> ExerciseSetsAndRepsModel::getExercisesFromDB()
{
    QList<WorkoutExerciseEntity*> dbExerciseList;

    QSqlQuery query;
    // Show only exercises for currently active workout template.
    bool result = query.prepare("SELECT DISTINCT e.id_exercise, e.name "
                  "FROM exercise e "
                  "INNER JOIN workout_template_exercises wte ON wte.id_exercise = e.id_exercise AND wte.id_workout_template = :id_workout_template "
                  "ORDER BY wte.delta ASC");

    if (!result) {
        qWarning() << "Error preparing query to get exercise list for active workout template.";
        qWarning() << query.lastError();
        return dbExerciseList;
    }

    AppState* appState = AppState::getInstance();
    User* user = appState->getCurrentUser();
    query.bindValue(":id_user", user->getId());
    query.bindValue(":id_workout_template", user->getLastIdWorkoutTemplate());

    result = query.exec();
    if (!result) {
        qWarning() << "Error getting exercise list for active workout template.";
        qWarning() << query.lastError();
        return dbExerciseList;
    }

    while (query.next()) {
        WorkoutExerciseEntity* exercise = new WorkoutExerciseEntity;
        exercise->idExercise = query.value("id_exercise").toInt();
        exercise->name = query.value("name").toString();
        dbExerciseList.append(exercise);
    }

    return dbExerciseList;
}

void ExerciseSetsAndRepsModel::init()
{
    beginResetModel();
    clear();
    exerciseList = getExercisesFromDB();
    endResetModel();
}

QVariant ExerciseSetsAndRepsModel::getSettingsProperties(const QModelIndex &index) const
{
    QVariantMap settingsMap;

    settingsMap.insert("cellType", "text");
    settingsMap.insert("label", data(index, ExerciseSetsAndRepsModel::ExerciseSetsAndRepsNameRole));
    settingsMap.insert("section", tr("EXERCISE SETS AND REPS"));
    settingsMap.insert("icon", "");
    settingsMap.insert("showIcon", false);
    settingsMap.insert("showAccessory", true);
    settingsMap.insert("accessoryIcon", ICON_CHEVRON_RIGHT);
    settingsMap.insert("value", "");
    settingsMap.insert("hiddenValue", data(index, ExerciseSetsAndRepsModel::ExerciseSetsAndRepsIDRole));
    settingsMap.insert("nextPageId", SETTINGS_PAGE_EXERCISE_SETS_AND_REPS);

    return settingsMap;
}

QVariant ExerciseSetsAndRepsModel::getSettingsCellType(const QModelIndex &index) const
{
    Q_UNUSED(index;)
    return QString("text");
}

QVariant ExerciseSetsAndRepsModel::getSettingsSection(const QModelIndex &index) const
{
    Q_UNUSED(index;)
    return tr("EXERCISE SETS AND REPS");
}

void ExerciseSetsAndRepsModel::refresh()
{
    beginResetModel();
    endResetModel();
}
