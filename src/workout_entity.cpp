#include "workout_entity.h"
#include "workout_exercise_entity.h"

const qreal WorkoutEntity::initialUserWeight = 75;

WorkoutEntity::~WorkoutEntity() {
    qDeleteAll(exercises);
    exercises.clear();
}
