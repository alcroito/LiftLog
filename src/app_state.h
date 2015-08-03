#ifndef APPSTATE_H
#define APPSTATE_H

#include <QObject>
#include <QSharedPointer>

class User;
class QScreen;
class QtQuickControlsApplication;
class WorkoutModel;

class AppState : public QObject
{
    Q_OBJECT
    Q_PROPERTY(User* currentUser READ getCurrentUser WRITE setCurrentUser NOTIFY currentUserChanged)
    Q_PROPERTY(WorkoutModel* currentWorkoutModel READ getCurrentWorkoutModel WRITE setCurrentWorkoutModel NOTIFY currentWorkoutModelChanged)
    Q_PROPERTY(qint32 windowWidth READ getWindowWidth WRITE setWindowWidth NOTIFY windowWidthChanged)
    Q_PROPERTY(qint32 windowHeight READ getWindowHeight WRITE setWindowHeight NOTIFY windowHeightChanged)
public:
    explicit AppState(QObject *parent = 0);

    User* getCurrentUser();
    void setCurrentUser(User* value);

    WorkoutModel* getCurrentWorkoutModel();
    void setCurrentWorkoutModel(WorkoutModel* model);

    void loadCurrentUser();

    qint32 getWindowWidth() const;
    void setWindowWidth(const qint32 &value);

    qint32 getWindowHeight() const;
    void setWindowHeight(const qint32 &value);

    void figureOutBestWindowSize(QScreen* screen);
    void computeScaleFactors(QtQuickControlsApplication* app, QScreen* screen);

    static AppState* getInstance() { return instance; }

signals:
    void currentUserChanged();
    void windowWidthChanged();
    void windowHeightChanged();
    void currentWorkoutModelChanged();

public slots:
    qint64 getActiveUserId();
    bool isActiveUserSet();
    void saveCurrentUser();
    void loadActiveUserOnDBInit();
    void clearActiveUserOnDBClose();
private:
    QSharedPointer<User> current_user;
    QSharedPointer<WorkoutModel> currentWorkoutModel;
    qint32 windowWidth;
    qint32 windowHeight;
    static AppState* instance;
};

#endif // APPSTATE_H
