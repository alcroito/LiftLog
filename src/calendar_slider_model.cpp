#include "calendar_slider_model.h"
#include "calendar_model.h"

#include <QDebug>
#include <QQmlEngine>

CalendarSliderModel::CalendarSliderModel(QObject *parent) : QAbstractListModel(parent), currentMiddleMonth(QDateTime::currentDateTimeUtc().date())
{
    generateModels();
}

void CalendarSliderModel::generateModels() {
    QList<QDate> dates;
    QDate prevDate = currentMiddleMonth.addMonths(-1);
    QDate nextDate = currentMiddleMonth.addMonths(1);

    dates.append(prevDate);
    dates.append(currentMiddleMonth);
    dates.append(nextDate);

    beginResetModel();
    for (int i = 0; i < dates.count(); ++i) {
        calendarModels.append(QSharedPointer<CalendarModel>(new CalendarModel(dates[i])));
    }
    endResetModel();
}

QDate CalendarSliderModel::goPrevMonth()
{
    currentMiddleMonth = currentMiddleMonth.addMonths(-1);
    QDate prevDate = currentMiddleMonth.addMonths(-1);

    beginInsertRows(QModelIndex(), 0, 0);
    calendarModels.prepend(QSharedPointer<CalendarModel>(new CalendarModel(prevDate)));
    endInsertRows();

    beginRemoveRows(QModelIndex(), rowCount() - 1, rowCount() - 1);
    calendarModels.removeLast();
    endRemoveRows();

    return currentMiddleMonth;
}

QDate CalendarSliderModel::goNextMonth()
{
    currentMiddleMonth = currentMiddleMonth.addMonths(1);
    QDate nextDate = currentMiddleMonth.addMonths(1);

    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    calendarModels.append(QSharedPointer<CalendarModel>(new CalendarModel(nextDate)));
    endInsertRows();

    beginRemoveRows(QModelIndex(), 0, 0);
    calendarModels.removeFirst();
    endRemoveRows();

    return currentMiddleMonth;
}

QVariant CalendarSliderModel::getModel(int indexId)
{
    QModelIndex modelIndex = index(indexId, 0);
    QVariant item = data(modelIndex, ModelRole);
    return item;
}

void CalendarSliderModel::refresh() {
    for (int i = 0; i < calendarModels.count(); ++i) {
        calendarModels[i]->refresh();
    }
}


int CalendarSliderModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return calendarModels.count();
}

QVariant CalendarSliderModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= calendarModels.count())
            return QVariant();

    const QSharedPointer<CalendarModel> calendarModel = calendarModels[index.row()];
    if (role == ModelRole) {
        QQmlEngine::setObjectOwnership(calendarModel.data(), QQmlEngine::CppOwnership);
        return QVariant::fromValue(calendarModel.data());
    }
    else if (role == DateRole) {
        return calendarModel->getDate();
    }
    return QVariant();
}

QHash<int, QByteArray> CalendarSliderModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[ModelRole] = "monthModel";
    roles[DateRole] = "monthDate";
    return roles;
}
