#include "workout_template_list_model.h"
#include <QDebug>

WorkoutTemplateListModel::WorkoutTemplateListModel(QObject *parent) : QSqlTableModel(parent)
{
    this->setTable("workout_template");
    this->setEditStrategy(QSqlTableModel::OnManualSubmit);
    this->select();
}

QHash<int, QByteArray> WorkoutTemplateListModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[IdRole] = "idWorkoutTemplate";
    roles[NameRole] = "name";
    return roles;
}

QVariant WorkoutTemplateListModel::data(const QModelIndex & index, int role) const
{
    QVariant value;
    if (role < Qt::UserRole) {
        value = QSqlQueryModel::data(index, role);
    }
    else {
        int columnIndex = role - Qt::UserRole - 1;
        QModelIndex modelIndex = this->index(index.row(), columnIndex);
        value = QSqlQueryModel::data(modelIndex, Qt::DisplayRole);
    }
    return value;
}

QVariant WorkoutTemplateListModel::getItemDataForIndexAndRole(int rowIndex, int role) const
{
    QVariant value;
    int columnIndex = role - Qt::UserRole - 1;
    QModelIndex modelIndex = this->index(rowIndex, columnIndex);
    value = QSqlQueryModel::data(modelIndex, Qt::DisplayRole);
    return value;
}
