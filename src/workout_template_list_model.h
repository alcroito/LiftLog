#ifndef WORKOUTTEMPLATELISTMODEL_H
#define WORKOUTTEMPLATELISTMODEL_H

#include <QObject>
#include <QSqlTableModel>

class WorkoutTemplateListModel : public QSqlTableModel
{
    Q_OBJECT
    Q_ENUMS(WorkoutTemplateRoles)
public:
    explicit WorkoutTemplateListModel(QObject *parent = 0);

    enum WorkoutTemplateRoles {
        IdRole = Qt::UserRole + 1,
        NameRole
    };

    QHash<int, QByteArray> roleNames() const;
    QVariant data(const QModelIndex &index, int role) const;
signals:

public slots:
    QVariant getItemDataForIndexAndRole(int rowIndex, int role) const;
};

#endif // WORKOUTTEMPLATELISTMODEL_H
