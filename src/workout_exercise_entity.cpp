#include "workout_exercise_entity.h"
#include "workout_set_entity.h"
#include <QDebug>

WorkoutExerciseEntity::~WorkoutExerciseEntity() {
    qDeleteAll(setsAndReps);
    setsAndReps.clear();
}
