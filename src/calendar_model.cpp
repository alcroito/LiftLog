#include "calendar_model.h"
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include "app_state.h"
#include "user.h"

CalendarModel::CalendarModel(QObject *parent) : QAbstractListModel(parent), currentMonth(QDate::currentDate())
{
    populateDatesListWithGivenMonth();
}

void CalendarModel::populateDatesListWithGivenMonth() {
    QDate currentDate = currentMonth;
    QMultiHash<QDate, qint64> datesToWorkouts = getWorkoutDates();

    // Get days in month.
    int daysInMonth = currentDate.daysInMonth();

    // Set the date to the first day of this month.
    currentDate.setDate(currentDate.year(), currentDate.month(), 1);

    // Get day of week to know how many days of previous month to show.
    int dayOfWeek = currentDate.dayOfWeek();

    // Add previous month days.
    int prevDaysToShow = dayOfWeek;
    QDate prevDate = currentDate;
    for (int i = 0; i < prevDaysToShow; i++) {
        prevDate = prevDate.addDays(-1);
        CalendarDate newDate(prevDate, "gray");
        addCalendarDate(newDate);
    }

    // Add current month days.
    for (int i = 0; i < daysInMonth; ++i) {
        CalendarDate newDate(currentDate);
        if (datesToWorkouts.contains(currentDate)) {
            newDate.state = "active";
        }
        addCalendarDate(newDate);
        currentDate = currentDate.addDays(1);
    }

    // Add next month days.
    QDate nextDate = currentDate;
    int additionalDaysToShow = 7 - currentDate.dayOfWeek();
    for (int i = 0; i < additionalDaysToShow; i++) {
        CalendarDate newDate(nextDate, "gray");
        addCalendarDate(newDate);
        nextDate = nextDate.addDays(1);
    }
}

QString getLastExecutedQuery(const QSqlQuery& query)
{
    QString str = query.lastQuery();
    qDebug() << query.boundValues();
    QMapIterator<QString, QVariant> it(query.boundValues());
    while (it.hasNext()) {
        it.next();
        str.replace(it.key(),it.value().toString());
    }
    return str;
}

QMultiHash<QDate, qint64> CalendarModel::getWorkoutDates() {
    QMultiHash<QDate, qint64> datesToWorkouts;

    QSqlQuery query;
    QString queryString = "SELECT id_workout, date_started "
                          "FROM user_workout "
                          "WHERE id_user = :id_user AND strftime('%Y-%m', date_started) = strftime('%Y-%m', '%1')";
    queryString = queryString.arg(currentMonth.toString("yyyy-MM-dd"));
    query.prepare(queryString);

    qint64 userId = AppState::getInstance()->getCurrentUser()->getId();
    query.bindValue(":id_user", userId);

    bool result = query.exec();
    if (!result) {
        qDebug() << "Error getting workout dates";
        qDebug() << query.lastError();
        return datesToWorkouts;
    }

    while (query.next()) {
        qint64 id = query.value(0).toInt();
        QDate date = query.value(1).toDateTime().date();
        datesToWorkouts.insert(date, id);
    }
    return datesToWorkouts;
}

void CalendarModel::addCalendarDate(const CalendarDate& calendarDate)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    calendarDates << calendarDate;
    endInsertRows();
}

void CalendarModel::switchMonth(QDate newDate) {
    beginRemoveRows(QModelIndex(), 0, rowCount());
    calendarDates.clear();
    endRemoveRows();

    currentMonth = newDate;
    populateDatesListWithGivenMonth();

}

void CalendarModel::goPrevMonth()
{
    currentMonth = currentMonth.addMonths(-1);
    switchMonth(currentMonth);
}

void CalendarModel::goNextMonth()
{
    currentMonth = currentMonth.addMonths(1);
    switchMonth(currentMonth);
}

QDate CalendarModel::getDate()
{
    return currentMonth;
}

int CalendarModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return calendarDates.count();
}

QVariant CalendarModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= calendarDates.count())
            return QVariant();

    const CalendarDate& calendarDate = calendarDates[index.row()];
    if (role == TextRole)
        return calendarDate.getText();
    else if (role == StateRole)
        return calendarDate.state;
    return QVariant();
}

QHash<int, QByteArray> CalendarModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[TextRole] = "text";
    roles[StateRole] = "state";
    return roles;
}

QString CalendarDate::getText() const
{
    return QVariant(date.day()).toString();
}

QString CalendarDate::getState() const
{
    return state;
}
