#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QSqlDatabase>
#include <QObject>


class DBManager: public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isFirstLaunch READ isFirstLaunch WRITE setIsFirstLaunch NOTIFY isFirstLaunchChanged)
public:
    DBManager(QObject *parent = 0);
    void init();

    bool isFirstLaunch();
    void setIsFirstLaunch(float new_value);

    QString getDBFileName();
    QString getDBFilePath();

    bool installDBTables();
    void deleteDBFile();
    bool copyInitDB(QString destination_path);

Q_SIGNALS:
    void isFirstLaunchChanged();
    void dbInitialized();
    void dbDestroyed();

public slots:
    void deleteDBFileAndReInit();

private:
    QSqlDatabase db;
    bool ok;
    bool firstLaunch;
};

#endif // DBMANAGER_H
