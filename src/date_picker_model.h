#ifndef DATEPICKERMODEL_H
#define DATEPICKERMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QDate>
#include <QSharedPointer>


class DatePickerModel : public QAbstractListModel
{
    Q_OBJECT
    Q_ENUMS(CalendarSliderRoles)
public:
    enum DatePickerRoles {
        TextRole = Qt::UserRole + 1,
        DateRole
    };
    explicit DatePickerModel(QObject *parent = 0);

    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

    void generateInitialDateList();
public slots:
    void goNextEntry();
    void goPrevEntry();
    qint32 getMiddleIndex();
    void update(qint32 currentIndex);
    QDate getDate(qint32 dateIndex);
    qint32 getCurrentDateIndex();
    void initWithDate(QDate date);
    qint32 getDateIndex(QDate date);
protected:
    QHash<int, QByteArray> roleNames() const;

private:
    QList<QDate> dateList;
    qint32 itemCountToPrepare;
};

#endif // DATEPICKERMODEL_H
