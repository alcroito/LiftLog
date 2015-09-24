#ifndef USER_H
#define USER_H

#include <QObject>
#include <QString>


class User : public QObject
{
    Q_OBJECT
    Q_ENUMS(WeightSystem)
    Q_PROPERTY(qint64 id READ getId WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString name READ getName WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(WeightSystem weightSystem READ getWeightSystem WRITE setWeightSystem NOTIFY weightSystemChanged)
    Q_PROPERTY(bool autoAddWeight READ getAutoAddWeight WRITE setAutoAddWeight NOTIFY autoAddWeightChanged)
    Q_PROPERTY(qint64 lastIdWorkoutTemplate READ getLastIdWorkoutTemplate WRITE setLastIdWorkoutTemplate NOTIFY lastIdWorkoutTemplateChanged)
    Q_PROPERTY(qint64 nextWorkoutTemplateDay READ getNextWorkoutTemplateDay WRITE setNextWorkoutTemplateDay NOTIFY nextWorkoutTemplateDayChanged)
    Q_PROPERTY(bool timerEnabled READ getTimerEnabled WRITE setTimerEnabled NOTIFY timerEnabledChanged)
    Q_PROPERTY(qreal timerSoundVolume READ getTimerSoundVolume WRITE setTimerSoundVolume NOTIFY timerSoundVolumeChanged)

public:
    explicit User(QObject *parent = 0);

    enum WeightSystem {
        Metric = 0,
        Imperial
    };

    QString getName() const;
    void setName(const QString &value);

    User::WeightSystem getWeightSystem() const;
    void setWeightSystem(const WeightSystem value);

    bool getAutoAddWeight() const;
    void setAutoAddWeight(bool value);

    qint64 getId() const;
    void setId(const qint64 &value);

    qint64 getLastIdWorkoutTemplate() const;
    void setLastIdWorkoutTemplate(const qint64 &value);

    qint64 getNextWorkoutTemplateDay() const;
    void setNextWorkoutTemplateDay(const qint64 &value);

    qreal getUserWeight();

    static qint64 getNextUserId();

    bool loadById(qint64 idUser);
    bool save();

    void clear();

    bool getTimerEnabled() const;
    void setTimerEnabled(bool value);

    qreal getTimerSoundVolume() const;
    void setTimerSoundVolume(const qreal &value);

    bool copyDefaultPlatesIntoUserPlatesTable();
    bool copyDefaultBarbellsIntoUserPlatesTable();
    bool copyDefaultExerciseCustomisationsIntoUserExerciseCustomisationsTable();
signals:
    void nameChanged();
    void weightSystemChanged(User::WeightSystem);
    void autoAddWeightChanged();
    void idChanged();
    void lastIdWorkoutTemplateChanged();
    void nextWorkoutTemplateDayChanged();
    void timerEnabledChanged(bool);
    void timerSoundVolumeChanged(qreal);

private:
    qint64 id;
    QString name;
    WeightSystem weightSystem;
    bool autoAddWeight;
    bool timerEnabled = true;
    qreal timerSoundVolume = 30;
    qint64 lastIdWorkoutTemplate;
    qint64 nextWorkoutTemplateDay;
};
Q_DECLARE_METATYPE(User::WeightSystem)

#endif // USER_H
