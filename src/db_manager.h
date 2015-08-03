#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QSqlDatabase>
#include <QObject>

// Rollbacks a database transaction when object goes out of scope, unless explicitly commited.
class DBTransaction {
public:
    DBTransaction();
    ~DBTransaction();
    void commit();
    void rollback();

private:
    bool commited;
    bool rolledBack;
};

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

    static DBManager* getInstance() { return instance; }

signals:
    void isFirstLaunchChanged();
    void dbInitialized();
    void dbDestroyed();

public slots:
    void deleteDBFileAndReInit();

private:
    QSqlDatabase db;
    bool ok;
    bool firstLaunch;
    static DBManager* instance;
};

#endif // DBMANAGER_H
