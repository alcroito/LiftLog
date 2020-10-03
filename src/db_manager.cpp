#include "db_manager.h"
#include <QDir>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QStandardPaths>
#include <QFile>
#include <QDir>

DBManager* DBManager::instance = 0;

DBManager::DBManager(QObject *parent) : QObject(parent), db(QSqlDatabase::addDatabase("QSQLITE")), firstLaunch(true)
{
    instance = this;
}

void DBManager::init() {
    QString db_path = getDBFilePath();
    qDebug() << "DB Location: " <<  db_path;

    bool db_file_exists = QFile::exists(db_path);
    if (!db_file_exists) {
        qDebug() << "No initial DB file found.";
        db_file_exists = copyInitDB(db_path);
    }
    if (!db_file_exists) {
        return;
    }

    db.setDatabaseName(db_path);
    ok = db.open();
    if (!ok) {
        qDebug() << "Error opening app DB file.";
        qDebug() << db.lastError();
        return;
    }

    qDebug() << "DB opened succesfully.";
    qDebug() << "DB file is located at: " << db_path;
    QStringList tables = db.tables();
    if (!tables.empty()) {
        qDebug() << "DB is properly populated.";
        firstLaunch = false;
        emit dbInitialized();
    }
    else {
        qDebug() << "DB set up failed";
    }
}

bool DBManager::copyInitDB(QString destination_path) {
    QFile initial_db_file_path(":/assets/db/liftlog.sqlite3");
    qDebug() << "Initial DB file present: " << initial_db_file_path.exists();
    bool result = initial_db_file_path.copy(destination_path);
    if (!result) {
        qDebug() << "Could not copy initial DB file.";
        qDebug() << initial_db_file_path.errorString();
    } else {
        QFile app_db_file_path(destination_path);
        bool permissions_set = app_db_file_path.setPermissions(QFile::ReadOwner | QFile::WriteOwner | QFile::ReadUser | QFile::WriteUser | QFile::ReadGroup | QFile::WriteGroup | QFile::ReadOther | QFile::WriteOther);
        if (!permissions_set) {
            qDebug() << "Could not make copied DB file writable";
            result = false;
        }
        else {
            qDebug() << "Copied inital DB to be used as the app DB.";
        }
    }

    return result;
}

QString DBManager::getLastExecutedQuery(const QSqlQuery &query)
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

bool DBManager::isFirstLaunch() { return firstLaunch; }

void DBManager::setIsFirstLaunch(float new_value) { firstLaunch = new_value; emit isFirstLaunchChanged(); }

bool DBManager::installDBTables() {
    qDebug() << "Installing initial DB schema.";
    QFile file(":/assets/db/db_initial_install.sql");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Error opening sql install file.";
        return false;
    }

    QByteArray file_contents = file.readAll();
    QList<QByteArray> install_statements = file_contents.split(';');
    for (int i = 0; i < install_statements.size(); ++i) {
        QSqlQuery query;

        if (install_statements[i].size() < 2) continue;

        bool result = query.exec(install_statements[i]);
        qDebug() << install_statements[i];
        if (!result) {
            qDebug() << "Error running initial sql install.";
            qDebug() << query.lastError();
            return false;
        }
    }

    qDebug() << "Schema installed successfully.";
    return true;
}

void DBManager::deleteDBFileAndReInit() {
    deleteDBFile();
    init();
    qDebug() << "Re-initialized DB.";
}

QString DBManager::getDBFileName() {
    return "liftlog.sqlite3";
}

QString DBManager::getDBFilePath() {
    QString db_path;
    QString db_file_name = getDBFileName();

#if TARGET_OS_IPHONE && TARGET_IPHONE_SIMULATOR
    QString folder_path = QDir::currentPath() + QDir::separator() + "Documents";
    QDir dir(folder_path);

    qDebug() << "DB for ios simulator case";
    if (!dir.exists()) {
        qDebug() << "DB folder does not exist. Creating it.";
        bool folder_created = dir.mkpath(folder_path);
        if (folder_created) {
            qDebug() << "Created successfully.";
        } else {
            qDebug() << "Creation failed.";
        }
    } else {
        qDebug() << "DB folder exists.";
    }

    db_path = folder_path + QDir::separator() + db_file_name;

#elif defined(Q_OS_IOS)
    qDebug() << "DB for ios device case";
    QStringList locations = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation);
    qDebug() << locations;
    QString folder_path = locations.value(0);
    db_path = folder_path + QDir::separator() + db_file_name;
#elif defined(Q_OS_OSX) || defined(Q_OS_LINUX) || defined(Q_OS_WIN)
    db_path = QDir::currentPath() + QDir::separator() + db_file_name;
#elif defined(Q_OS_ANDROID)
    QStringList locations = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation);
    qDebug() << locations;
    db_path = locations.value(0) + QDir::separator() + db_file_name;
#else
#error "No support for OS yet."
#endif

    return db_path;
}

void DBManager::deleteDBFile()
{
    qDebug() << "Removing DB file.";
    db.close();
    QString db_path = getDBFilePath();
    QFile file(db_path);
    file.remove();
    ok = false;
    setIsFirstLaunch(true);
    emit dbDestroyed();
}

DBTransaction::DBTransaction() : commited(false), rolledBack(false) {
    bool result = QSqlDatabase::database().transaction();
    if (!result) {
        qDebug() << "Error starting transaction.";
    }
}

DBTransaction::~DBTransaction() {
    rollback();
}

void DBTransaction::commit() {
    if (!commited && !rolledBack) {
        bool result = QSqlDatabase::database().commit();
        if (!result) {
            qDebug() << "Error committing transaction.";
            qDebug() << QSqlDatabase::database().lastError();
        }
        commited = true;
    }
}

void DBTransaction::rollback() {
    if (!commited && !rolledBack) {
        bool result = QSqlDatabase::database().rollback();
        if (!result) {
            qDebug() << "Error rolling back transaction.";
            qDebug() << QSqlDatabase::database().lastError();
        }
        rolledBack = true;
    }
}
