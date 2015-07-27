BEGIN TRANSACTION;
CREATE TABLE "workout_template_exercises" (
        `id_workout_template`	INTEGER,
        `day`	INTEGER DEFAULT 0,
        `id_exercise`	INTEGER,
        `delta`	INTEGER DEFAULT 0,
        FOREIGN KEY(`id_workout_template`) REFERENCES workout_template ( id_workout_template ),
        FOREIGN KEY(`id_exercise`) REFERENCES exercise ( id_exercise )
);
CREATE TABLE `workout_template` (
        `id_workout_template`	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
        `name`	TEXT
);
CREATE TABLE "user_workout_exercise_weight" (
        `id_workout`	INTEGER,
        `id_exercise`	INTEGER,
        `delta`	INTEGER,
        `weight`	NUMERIC,
        FOREIGN KEY(`id_workout`) REFERENCES user_workout ( id_workout ),
        FOREIGN KEY(`id_exercise`) REFERENCES exercise(id_exercise)
);
CREATE TABLE "user_workout_exercise_stats" (
        `id_workout`	INTEGER,
        `id_exercise`	INTEGER,
        `delta`	INTEGER,
        `set_number`	INTEGER,
        `reps_done`	INTEGER,
        FOREIGN KEY(`id_workout`) REFERENCES workout ( id_workout ),
        FOREIGN KEY(`id_exercise`) REFERENCES exercise(id_exercise)
);
CREATE TABLE "user_workout" (
        `id_workout`	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
        `id_user`	INTEGER,
        `date_started`	TEXT,
        `date_ended`	TEXT,
        `last_updated`	TEXT,
        `user_weight`	NUMERIC,
        FOREIGN KEY(`id_user`) REFERENCES user(id_user)
);
CREATE TABLE `user` (
        `id_user`	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
        `name`	TEXT,
        `weight_system`	INTEGER DEFAULT 0,
        `auto_add_weight`	INTEGER DEFAULT 1,
        `last_id_workout_template`	INTEGER,
        `last_workout_template_day`	INTEGER
);
CREATE TABLE `set_and_rep` (
        `id_set_and_rep`	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
        `set_count`	INTEGER,
        `rep_count`	INTEGER
);
CREATE TABLE "exercise" (
        `id_exercise`	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,
        `name`	TEXT,
        `tags`	TEXT,
        `id_default_set_and_rep`	INTEGER,
        `default_weight`	NUMERIC DEFAULT 20,
        `default_weight_increment`	NUMERIC DEFAULT 5,
        FOREIGN KEY(`id_default_set_and_rep`) REFERENCES set_and_rep ( id_set_rep )
);
CREATE TABLE "settings" (
        `active_id_user`	INTEGER
);
COMMIT;
