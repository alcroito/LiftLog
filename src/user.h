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
    Q_PROPERTY(WeightSystem weightSystem READ weightSystem WRITE setWeightSystem NOTIFY weightSystemChanged)
    Q_PROPERTY(bool autoAddWeight READ getAutoAddWeight WRITE setAutoAddWeight NOTIFY autoAddWeightChanged)

public:
    explicit User(QObject *parent = 0);

    enum WeightSystem {
        Metric = 0,
        Imperial
    };

    QString getName() const;
    void setName(const QString &value);

    User::WeightSystem weightSystem() const;
    void setWeightSystem(const WeightSystem value);

    bool getAutoAddWeight() const;
    void setAutoAddWeight(bool value);

    qint64 getId() const;
    void setId(const qint64 &value);

    static qint64 getNextUserId();

    void clear();
signals:
    void nameChanged();
    void weightSystemChanged();
    void autoAddWeightChanged();
    void idChanged();
public slots:

private:
    qint64 id;
    QString name;
    WeightSystem weight_system;
    bool auto_add_weight;
    qint64 last_id_workout_template;
    qint64 last_workout_template_day;
};

#endif // USER_H
