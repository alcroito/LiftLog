#include "date_picker_model.h"
#include <QDebug>

DatePickerModel::DatePickerModel(QObject *parent) : QAbstractListModel(parent), itemCountToPrepare(20)
{
    generateInitialDateList();
}

void DatePickerModel::clear() {
    beginResetModel();
    dateList.clear();
    endResetModel();
}

void DatePickerModel::generateInitialDateList() {
    clear();
    QDate currentDate = QDate::currentDate();
    qint32 halfItemCount = itemCountToPrepare / 2;
    QDate leftEnd = currentDate.addDays(-halfItemCount);
    QDate rightEnd = currentDate.addDays(halfItemCount);
    for (QDate date = leftEnd; date != rightEnd; date = date.addDays(1)) {
        dateList.append(date);
    }
}

void DatePickerModel::initWithDate(QDate date) {
    clear();
    QDate currentDate = date;
    qint32 halfItemCount = itemCountToPrepare / 2;
    QDate leftEnd = currentDate.addDays(-halfItemCount);
    QDate rightEnd = currentDate.addDays(halfItemCount);
    beginResetModel();
    for (QDate date = leftEnd; date != rightEnd; date = date.addDays(1)) {
        dateList.append(date);
    }
    endResetModel();
}

void DatePickerModel::goNextEntry()
{
    beginRemoveRows(QModelIndex(), 0, 0);
    dateList.removeFirst();
    endRemoveRows();

    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    dateList.append(dateList.last().addDays(1));
    endInsertRows();
}

void DatePickerModel::goPrevEntry()
{
    beginRemoveRows(QModelIndex(), rowCount() - 1, rowCount() - 1);
    dateList.removeLast();
    endRemoveRows();

    beginInsertRows(QModelIndex(), 0, 0);
    dateList.prepend(dateList.first().addDays(-1));
    endInsertRows();
}

qint32 DatePickerModel::getMiddleIndex() {
    return rowCount() / 2 - 1;
}

qint32 DatePickerModel::getCurrentDateIndex() {
    qint32 i = dateList.indexOf(QDate::currentDate());
    if (i != -1) {
        return i;
    }
    return getMiddleIndex();
}

qint32 DatePickerModel::getDateIndex(QDate date) {
    qint32 i = dateList.indexOf(date);
    if (i != -1) {
        return i;
    }
    return getMiddleIndex();
}

/**
 * @brief DatePickerModel::update
 * @param currentIndex
 *
 * Updates dateList to contain more elements at its ends, depending on which end is the value of currentIndex closer to.
 */
void DatePickerModel::update(qint32 currentIndex) {
    qint32 quarter = itemCountToPrepare / 4;
    qint32 threeQuarters = quarter * 3;

    if (currentIndex <= quarter - 1) {
        beginRemoveRows(QModelIndex(), threeQuarters, rowCount() - 1);
        dateList.erase(dateList.begin() + (threeQuarters), dateList.end());
        endRemoveRows();

        beginInsertRows(QModelIndex(), 0, quarter - 1);
        for (int i = 0; i < quarter; ++i) {
            dateList.prepend(dateList.first().addDays(-1));
        }
        endInsertRows();
    }

    if (currentIndex >= threeQuarters - 1) {
        beginRemoveRows(QModelIndex(), 0, quarter - 1);
        dateList.erase(dateList.begin(), dateList.begin() + quarter);
        endRemoveRows();

        beginInsertRows(QModelIndex(), rowCount(), rowCount() + quarter - 1);
        for (int i = 0; i < quarter; ++i) {
            dateList.append(dateList.last().addDays(1));
        }
        endInsertRows();
    }
}

int DatePickerModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return dateList.count();
}

QVariant DatePickerModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= dateList.count())
            return QVariant();

    const QDate date = dateList[index.row()];
    if (role == TextRole) {
        return date.toString("ddd MMM dd yyyy");
    }
    else if (role == DateRole) {
        return date;
    }
    return QVariant();
}

QDate DatePickerModel::getDate(qint32 dateIndex) {
    return data(index(dateIndex), DateRole).toDate();
}

QHash<int, QByteArray> DatePickerModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[TextRole] = "text";
    roles[DateRole] = "date";
    return roles;
}
