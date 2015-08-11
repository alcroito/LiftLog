#ifndef SETANDREPMODEL_H
#define SETANDREPMODEL_H

#include <QObject>
#include <QAbstractListModel>

class SetAndRep {
public:
    qint64 id;
    qint32 setCount;
    qint32 repCount;
    QString state;
    qint32 delta;

    qint64 getId() const { return id; }
    qint32 getSetCount() const { return setCount; }
    qint32 getRepCount() const { return repCount; }
    QString getState() const { return state; }
    qint32 getDelta() const { return delta; }
    QString getText() const { return QString("%1x%2").arg(getSetCount()).arg(getRepCount()); }
    bool isValid() const { return getId() != 0; }
};

class SetAndRepModel : public QAbstractListModel
{
    Q_OBJECT
    Q_ENUMS(SetAndRepRoles)
public:
    enum SetAndRepRoles {
        TextRole = Qt::UserRole + 1,
        IdRole,
        StateRole,
        SetCountRole,
        RepCountRole,
        DeltaRole
    };

    explicit SetAndRepModel(QObject *parent = 0);
    virtual ~SetAndRepModel();

    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

    static SetAndRep getSetAndRepById(qint64 id);
public slots:

    QList<SetAndRep> getDBSetAndRepList(qint64 initId = 0);
    void init();
    qint32 initWithSetAndRepIdAndReturnSetIndex(qint64 id = 0);
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    void resetStates();
protected:
    QHash<int, QByteArray> roleNames() const;

private:
    QList<SetAndRep> setAndRepList;
};

#endif // SETANDREPMODEL_H
