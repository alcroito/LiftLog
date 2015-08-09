#ifndef CALENDARSLIDERMODEL_H
#define CALENDARSLIDERMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QDate>
#include <QSharedPointer>

class CalendarModel;

class CalendarSliderModel : public QAbstractListModel
{
    Q_OBJECT
    Q_ENUMS(CalendarSliderRoles)
public:
    enum CalendarSliderRoles {
        ModelRole = Qt::UserRole + 1,
        DateRole
    };

    explicit CalendarSliderModel(QObject *parent = 0);

    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

    void generateModels();

public slots:
    QDate goPrevMonth();
    QDate goNextMonth();
    QVariant getModel(int indexId);

protected:
    QHash<int, QByteArray> roleNames() const;

private:
    QList<QSharedPointer<CalendarModel> > calendarModels;
    QDate currentMiddleMonth;
};

#endif // CALENDARSLIDERMODEL_H
