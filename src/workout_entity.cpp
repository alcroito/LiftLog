#include "workout_entity.h"
#include "workout_exercise_entity.h"

WorkoutEntity::~WorkoutEntity() { qDeleteAll(exercises); }
