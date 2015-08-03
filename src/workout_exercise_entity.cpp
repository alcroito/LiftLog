#include "workout_exercise_entity.h"
#include "workout_set_entity.h"

WorkoutExerciseEntity::~WorkoutExerciseEntity() { qDeleteAll(setsAndReps); }
