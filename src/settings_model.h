#ifndef SETTINGSMODEL_H
#define SETTINGSMODEL_H

#include <QObject>
#include <QAbstractItemModel>

class GenericTreeNode;
class SettingsModel : public QAbstractItemModel
{
    Q_OBJECT
    Q_ENUMS(SettingsRoles)
public:
    enum SettingsRoles {
        SettingsPropertiesRole = Qt::UserRole + 1,
        SettingsCellTypeRole,
        SettingsSectionRole,
    };

    explicit SettingsModel(QObject* parent = 0);
    ~SettingsModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;

    GenericTreeNode* getModelItem(const QModelIndex &index) const;

    static const QString PAGE_INITIAL;
    static const QString PAGE_WEIGHT;
    static const QString PAGE_PLATES;
    static const QString PAGE_BARBELL;

protected:
    virtual QHash<int, QByteArray> roleNames() const;

public slots:
    void clear();
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    void getSettingsData(QString page = PAGE_INITIAL);
    void cellClicked(int row);
    void cellSwitchValueChanged(int row, bool checked);
    void cellSliderValueChanged(int row, qreal value);
    void refresh();
    void prependNewCell();
signals:
    void switchToSettingsPage(QString pageId);

private:
    GenericTreeNode* root;
    QString currentPageId;
    QAbstractItemModel* innerModel = nullptr;
};

#endif // SETTINGSMODEL_H
