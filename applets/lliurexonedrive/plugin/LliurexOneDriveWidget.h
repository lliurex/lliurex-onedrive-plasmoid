#ifndef PLASMA_LLIUREX_ONEDRIVE_WIDGET_H
#define PLASMA_LLIUREX_ONEDRIVE_WIDGET_H

#include <QObject>
#include <QProcess>
#include <QPointer>
#include <KNotification>
#include <QDir>
#include <QFile>
#include <KIO/CommandLauncherJob>
#include "LliurexOneDriveWidgetUtils.h"

class QTimer;
class KNotification;
class LliurexOneDriveWidgetModel;

class LliurexOneDriveWidget : public QObject
{
    Q_OBJECT


    Q_PROPERTY(TrayStatus status READ status NOTIFY statusChanged)
    Q_PROPERTY(QString toolTip READ toolTip NOTIFY toolTipChanged)
    Q_PROPERTY(QString subToolTip READ subToolTip NOTIFY subToolTipChanged)
    Q_PROPERTY(QString iconName READ iconName NOTIFY iconNameChanged)
    Q_PROPERTY(QString oneDriveFolder READ oneDriveFolder NOTIFY oneDriveFolderChanged)
    Q_PROPERTY(bool syncStatus READ syncStatus NOTIFY syncStatusChanged)
    Q_PROPERTY(QString freeSpace READ freeSpace NOTIFY freeSpaceChanged)
    Q_PROPERTY(bool lliurexOneDriveOpen READ lliurexOneDriveOpen NOTIFY lliurexOneDriveOpenChanged)
    Q_PROPERTY(bool showSearchFiles READ showSearchFiles NOTIFY showSearchFilesChanged)
    Q_PROPERTY(LliurexOneDriveWidgetModel* model READ model CONSTANT)

    Q_ENUMS(TrayStatus)

public:
    LliurexOneDriveWidget(QObject *parent = nullptr);


public:
    /**
     * System tray icon states.
     */
    enum TrayStatus {
        ActiveStatus = 0,
        PassiveStatus
    };

public:
    TrayStatus status() const;
    void changeTryIconState (int state);
    void setStatus(TrayStatus status);

    QString oneDriveFolder() const;
    void setOneDriveFolder(const QString &oneDriveFolder);

    QString freeSpace() const;
    void setFreeSpace(const QString &freeSpace);

    bool syncStatus();
    void setSyncStatus(bool &syncStatus);

    QString toolTip() const;
    void setToolTip(const QString &toolTip);

    QString subToolTip() const;
    void setSubToolTip(const QString &subToolTip);

    QString iconName() const;
    void setIconName(const QString &name);

    bool lliurexOneDriveOpen();
    void setLliurexOneDriveOpen(bool);

    QStringList runCheckAccount();
    void isAlive();

    bool showSearchFiles();
    void setShowSearchFiles(bool);

    LliurexOneDriveWidgetModel *model() const;


public slots:
    
    void worker();
    void launchOneDrive();
    void openFolder();
    void manageSync();
    void openHelp();
    void getLatestFiles();
    void goToFile(const QString &filePath);

signals:
   
    void oneDriveFolderChanged();
    void syncStatusChanged();
    void freeSpaceChanged();
    void statusChanged();
    void toolTipChanged();
    void subToolTipChanged();
    void iconNameChanged();
    void lliurexOneDriveOpenChanged();
    void showSearchFilesChanged();

private:

    LliurexOneDriveWidgetUtils *m_utils;
    LliurexOneDriveWidgetModel *m_model = nullptr;
    QProcess *m_checkProcess=nullptr;
    QProcess *m_isDisplayProcess=nullptr;
    QProcess *m_isRunningProcess=nullptr;
    QProcess *m_isLliurexOneDriveOpen=nullptr;
    QProcess *m_getLatestFiles=nullptr;
    void plasmoidMode();
    void initPlasmoid();
    void updateWidget(QString subtooltip,QString icon);
    void showSyncNotification();
    QTimer *m_timer = nullptr;
    TrayStatus m_status = PassiveStatus;
    QString m_iconName = QStringLiteral("onedrive-pause");
    QString m_oneDriveFolder;
    QString m_freeSpace;
    QString userHome;
    bool m_syncStatus=false;
    QString m_toolTip;
    QString m_subToolTip;
    bool m_lliurexOneDriveOpen=false;
    QFile TARGET_FILE;
    bool isWorking=false;
    bool isRunning=false;
    int lastCheck=80;
    bool previousError=false;
    bool warning=false;
    bool checkExecuted=false;
    bool initClientStatus=false;
    bool changeSyncStatus=false;
    bool tryChangeStatus=false;
    bool m_showSearchFiles=false;
    QString previousErrorCode="";
    QPointer<KNotification> m_errorNotification;
    int countRepeatGeneralError=0;

private slots:

     void checkIsRunning();
     void isRunningProcessFinished(int exitCode,QProcess::ExitStatus exitStatus);
     void checkIsDisplayRunning();
     void isDisplayProcessFinished(int exitCode,QProcess::ExitStatus exitStatus);
     void checkStatus();
     void checkProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
     void checkIsLliurexOneDriveOpen();
     void isLliurexOneDriveOpenProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
     void getLatestFilesFinished(int exitCode, QProcess::ExitStatus exitStatus);

};

#endif // PLASMA_LLIUREX_ONEDRIVE_WIDGET_H
