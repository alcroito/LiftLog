#include "workout_exercise_entity.h"
#include "workout_set_entity.h"
#include <QDebug>

WorkoutExerciseEntity::~WorkoutExerciseEntity() {
    qDeleteAll(setsAndReps);
    setsAndReps.clear();
}

WorkoutExerciseEntity::ExerciseCategory WorkoutExerciseEntity::exerciseCategory()
{
    if (isPrimary()) return Barbell;
    else if (isAccessory()) return BodyWeight;
    else return Unknown;
}

qreal WorkoutExerciseEntity::getBarbellWeight()
{
    // @TODO Make this configurable.
    return 20.0;
}
