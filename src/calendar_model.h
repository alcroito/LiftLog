#ifndef CALENDARMODEL_H
#define CALENDARMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QDate>

class CalendarDate {
public:
    CalendarDate() {}
    CalendarDate(QString newState) : state(newState) {}
    CalendarDate(QDate newDate) : date(newDate) {}
    CalendarDate(QDate newDate, QString newState) : date(newDate), state(newState) {}
    QString getText() const;
    QString getState() const;
    QDate getDate() const;

    QDate date;
    QString state;
};

class CalendarModel : public QAbstractListModel
{
    Q_OBJECT
    Q_ENUMS(CalendarRoles)
public:
    enum CalendarRoles {
        TextRole = Qt::UserRole + 1,
        StateRole,
        DateRole
    };

    explicit CalendarModel(QObject *parent = 0);
    explicit CalendarModel(QDate date, QObject *parent = 0);
    virtual ~CalendarModel();
    void addCalendarDate(const CalendarDate& calendarDate);
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

    void populateDatesListWithGivenMonth();

    QMultiHash<QDate, qint64> getWorkoutDates();
public slots:
    void switchMonth(QDate newDate);
    void goPrevMonth();
    void goNextMonth();
    QDate getDate();
    void refresh();

protected:
    QHash<int, QByteArray> roleNames() const;

private:
    QList<CalendarDate> calendarDates;
    QDate currentMonth;
};

#endif // CALENDARMODEL_H
