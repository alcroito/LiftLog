#include "workout_set_entity.h"
#include <QDebug>

QString const WorkoutSetEntity::EMPTY_STATE = "empty";
QString const WorkoutSetEntity::BLINKING_STATE = "blinking";
QString const WorkoutSetEntity::CROSSED_STATE = "crossed";
QString const WorkoutSetEntity::ACTIVE_STATE = "active";
QString const WorkoutSetEntity::STANDARD_STATE = "standard";
const qint32 WorkoutSetEntity::NO_REPS_DONE = -1;
const qint64 WorkoutSetEntity::ID_NOT_SET = -1;

WorkoutSetEntity::~WorkoutSetEntity() {}
