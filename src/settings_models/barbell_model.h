#ifndef BARBELLMODEL_H
#define BARBELLMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include "../barbell.h"
#include "settings_interface.h"

class BarbellModel : public QAbstractListModel, public SettingsInterface
{
    Q_OBJECT
    Q_ENUMS(BarbellModelRoles)
    Q_INTERFACES(SettingsInterface)
public:
    enum BarbellModelRoles {
        BarbellWeightRole = Qt::UserRole + 1,
        BarbellMetricWeightRole,
        BarbellImperialWeightRole,
        BarbellExerciseNameRole
    };
    explicit BarbellModel(QObject *parent = 0);

    void init();

    QVariant getSettingsProperties(const QModelIndex &index) const;
    QVariant getSettingsCellType(const QModelIndex &index) const;
    QVariant getSettingsSection(const QModelIndex &index) const;
    void refresh();
    void cellTextInputValueChanged(int row, int textInputDelta, QString value);
    void resetToDefaults();

public slots:
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

    QList<Barbell> getBarbellsFromDB();
    bool saveUserBarbell(Barbell &b);
    void resetUserBarbells();
protected:
     QHash<int, QByteArray> roleNames() const;

signals:
    void switchToSettingsPage(QString pageId);

private:
    QList<Barbell> barbellList;
};

#endif // BARBELLMODEL_H
